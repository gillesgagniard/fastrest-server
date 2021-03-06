/*
 * Copyright (c) 2014 Gilles Gagniard <gilles@gagniard.org>
 *
 * This file is part of fastrest-server.
 *
 * fastrest-server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * fastrest-server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fastrest-server.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "http.hpp"
#include "logutils.hpp"
#include "dispatcher.hpp"
#include "threadpool.hpp"
#include <boost/asio/read_until.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <stdexcept>

namespace fastrest
{
  
  http_session::http_session(boost::asio::ip::tcp::socket s, threadpool& tpool, dispatcher& dispatcher) :
          _socket(std::move(s)),
          _tpool(tpool),
          _dispatcher(dispatcher),
          _content_type(CONTENT_TYPE_OTHER),
          _connection_close(false),
          _buffer_request(1024*1024), _buffer_response(1024*1024) // preallocate memory
  {
    
  }
  
  void http_session::start()
  {
    read_http_header();
  }
  
  void http_session::read_http_header()
  {
    auto self = shared_from_this();
    // read until CRLFCRLF -> end of http header
    boost::asio::async_read_until(_socket, _buffer_request, HTTP_HEADER_END, [this, self](boost::system::error_code ec, std::size_t length)
    {
      if (!ec)
      {
        size_t content_length = 0;
        
        std::istream is(&_buffer_request);
        std::string line;
        std::getline(is, line);
        size_t sep1 = line.find(" ");
        size_t sep2 = line.find(" ", sep1 + 1);
        if (sep1 == std::string::npos || sep2 == std::string::npos)
        {
          LOG_ERROR << "invalid request " << line;
          _connection_close = true;
          write_http_response(HTTP_CODE_BAD_REQUEST);
          return;
        }
        _method = line.substr(0, sep1);
        _uri = line.substr(sep1 + 1, sep2 - sep1 - 1);
        _version = line.substr(sep2 + 1, line.size() - 1); // without \r at the end !
        LOG_DEBUG << "request " << _method << " " << _uri << " " << _version;
        
        if (_version == HTTP_VERSION_10)
        {
          _connection_close = true;
          LOG_DEBUG << HTTP_VERSION_10 " -> connection close";
        }

        while (std::getline(is, line) && line.size() > 1) // more than just \r in line
        {
          // iterate over all headers
          size_t pos_separator = line.find(": ");
          if (pos_separator == std::string::npos)
          {
            LOG_ERROR << "invalid header " << line;
            _connection_close = true;
            write_http_response(HTTP_CODE_BAD_REQUEST);
            return;
          }
          line[pos_separator] = 0; // put \0 in place of the : separator
          line[line.size() - 1] = 0; // put \0 in place of the \r at the end
          size_t pos_separator_params = line.find("; ");
          if (pos_separator_params != std::string::npos)
          {
            line[pos_separator_params] = 0; // put \0 in place of the ; separator
          }
          char* attr_name = &line[0];
          char* attr_value = &line[pos_separator + 2];
          if (strncmp(attr_name, HTTP_HEADER_CONTENT_LENGTH, sizeof(HTTP_HEADER_CONTENT_LENGTH)) == 0)
          {
            content_length = atol(attr_value);
            LOG_DEBUG << "content length " << content_length;
          }
          else if (strncmp(attr_name, HTTP_HEADER_CONNECTION, sizeof(HTTP_HEADER_CONNECTION)) == 0 &&
                   strncmp(attr_value, HTTP_HEADER_CONNECTION_CLOSE, sizeof(HTTP_HEADER_CONNECTION_CLOSE)) == 0)
          {
            _connection_close = true;
            LOG_DEBUG << "connection close header";
          }
          else if (strncmp(attr_name, HTTP_HEADER_CONTENT_TYPE, sizeof(HTTP_HEADER_CONTENT_TYPE)) == 0)
          {
            LOG_DEBUG << "content type raw " << attr_value;
            if (strncmp(attr_value, HTTP_HEADER_CONTENT_TYPE_JSON, sizeof(HTTP_HEADER_CONTENT_TYPE_JSON)) == 0)
            {
              _content_type = CONTENT_TYPE_JSON;
            }
            else if (strncmp(attr_value, HTTP_HEADER_CONTENT_TYPE_TEXT, sizeof(HTTP_HEADER_CONTENT_TYPE_TEXT)) == 0)
            {
              _content_type = CONTENT_TYPE_TEXT;
            }
            else if (strncmp(attr_value, HTTP_HEADER_CONTENT_TYPE_XML, sizeof(HTTP_HEADER_CONTENT_TYPE_XML)) == 0)
            {
              _content_type = CONTENT_TYPE_XML;
            }
            else
            {
              _content_type = CONTENT_TYPE_OTHER;
            }
            LOG_DEBUG << "body content type " << _content_type;
          }
        }
        
        read_http_body(content_length);
      }
      else
      {
        LOG_ERROR << "error while reading http header " << ec;
      }
    });
  }
  
  void http_session::read_http_body(size_t body_length)
  {
    auto self = shared_from_this();
    boost::asio::async_read(_socket, _buffer_request, [this, self, body_length](boost::system::error_code ec, std::size_t length) { // condition
      if (!ec)
      {
        return body_length - _buffer_request.size();
      }
      else
      {
        return (size_t)0; // terminate ...
      }
    },
    [self, body_length](boost::system::error_code ec, std::size_t length) { // callback
      if (!ec)
      {
        std::string body(boost::asio::buffer_cast<const char*>(self->_buffer_request.data()), body_length);
        self->_buffer_request.consume(body_length); // consume the body

        LOG_DEBUG << "body : " << body;

        self->_tpool.run_task([self, body]() {self->_dispatcher.execute(self, std::move(body));});
 
        if (!self->_connection_close)
        {
          self->read_http_header();
        }
      }
      else
      {
        LOG_ERROR << "error while reading body " << ec;
      }
    });
  }
  
  void http_session::write_http_response(unsigned int code, ContentType_t content_type, const char* data, size_t data_size)
  {    
    auto self = shared_from_this();
    
    const char* reason = NULL;
    switch (code) {
    case HTTP_CODE_OK:
      reason = HTTP_REASON_OK;
      break;
    case HTTP_CODE_BAD_REQUEST:
      reason = HTTP_REASON_BAD_REQUEST;
      break;
    case HTTP_CODE_METHOD_NOT_ALLOWED:
      reason = HTTP_REASON_METHOD_NOT_ALLOWED;
      break;
    case HTTP_CODE_LENGTH_REQUIRED:
      reason = HTTP_REASON_LENGTH_REQUIRED;
      break;
    case HTTP_CODE_NOT_FOUND:
      reason = HTTP_REASON_NOT_FOUND;
      break;
    }
    
    std::ostream response(&_buffer_response);
    response << HTTP_VERSION_11 " " << code << " " << reason << HTTP_DELIMITER;
    if (data && data_size)
    {
      LOG_DEBUG << "reply data content type " << content_type << " data " << data;
      switch (content_type)
      {
      case CONTENT_TYPE_JSON:
        response << HTTP_HEADER_CONTENT_TYPE ": " HTTP_HEADER_CONTENT_TYPE_JSON HTTP_DELIMITER HTTP_HEADER_CONTENT_LENGTH ": " << data_size << HTTP_HEADER_END;
        break;
      case CONTENT_TYPE_TEXT:
        response << HTTP_HEADER_CONTENT_TYPE ": " HTTP_HEADER_CONTENT_TYPE_TEXT HTTP_DELIMITER HTTP_HEADER_CONTENT_LENGTH ": " << data_size << HTTP_HEADER_END;
        break;
      case CONTENT_TYPE_XML:
        response << HTTP_HEADER_CONTENT_TYPE ": " HTTP_HEADER_CONTENT_TYPE_XML HTTP_DELIMITER HTTP_HEADER_CONTENT_LENGTH ": " << data_size << HTTP_HEADER_END;
        break;
      default:
        LOG_ERROR << "unsupported content type";
        throw std::invalid_argument("unsupported content type");
      }
      response << data;
    }
    else
    {
      LOG_DEBUG << "reply no data";
      //response << HTTP_HEADER_CONTENT_TYPE ": " HTTP_HEADER_CONTENT_TYPE_TEXT HTTP_DELIMITER HTTP_HEADER_CONTENT_LENGTH ": " << sizeof(HTTP_BODY_NO_CONTENT) - 1 << HTTP_HEADER_END;
      //response << HTTP_BODY_NO_CONTENT;
      response << HTTP_HEADER_CONTENT_TYPE ": " HTTP_HEADER_CONTENT_TYPE_TEXT HTTP_DELIMITER HTTP_HEADER_CONTENT_LENGTH ": " << 0 << HTTP_HEADER_END;
    }
    boost::asio::async_write(_socket, _buffer_response, [self](boost::system::error_code ec, std::size_t length)
    {
      if (ec)
      {
        LOG_ERROR << "error while writing http response " << ec;
      }
      else
      {
        LOG_INFO << "done writing response";
      }
    });
  }
  
  http_server::http_server(boost::asio::io_service& io_service, boost::asio::ip::tcp::endpoint& endpoint, threadpool& tpool, dispatcher& dispatcher) :
      _acceptor(io_service, endpoint),
      _socket(io_service),
      _tpool(tpool),
      _dispatcher(dispatcher)
  {
    accept();
  }
  
  void http_server::accept()
  {
    LOG_INFO << "accepting incoming connections";
    _acceptor.async_accept(_socket, [this](boost::system::error_code ec)
    {
      if (!ec)
      {
        std::make_shared<http_session>(std::move(_socket), _tpool, _dispatcher)->start();
        // after move, _socket is ready to be used again as if it were freshly constructed !
      } else
      {
        LOG_ERROR << "error " << ec;
      }

      accept();
    });
  }

}
