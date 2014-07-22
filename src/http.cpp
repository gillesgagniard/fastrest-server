/* 
 * File:   http.cpp
 * Author: gilles
 * 
 * Created on 9 mars 2014, 23:40
 */

#include "http.hpp"
#include "logutils.hpp"
#include "dispatcher.hpp"
#include "threadpool.hpp"
#include <boost/asio/read_until.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

namespace fastrest
{
  
  http_session::http_session(boost::asio::ip::tcp::socket s, threadpool& tpool, dispatcher& dispatcher) :
          _socket(std::move(s)),
          _tpool(tpool),
          _dispatcher(dispatcher),
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
//        bool content_type_json = false;
        
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
//          else if (strncmp(attr_name, HTTP_HEADER_CONTENT_TYPE, sizeof(HTTP_HEADER_CONTENT_TYPE)) == 0 &&
//                   strncmp(attr_value, HTTP_HEADER_CONTENT_TYPE_JSON, sizeof(HTTP_HEADER_CONTENT_TYPE_JSON)) == 0)
//          {
//            content_type_json = true;
//            LOG_DEBUG << "content-type json ok";
//          }
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
        std::string body(boost::asio::buffer_cast<const char*>(self->_buffer_request.data()));
        self->_buffer_request.consume(body_length); // consume the body

        LOG_DEBUG << "body \"" << body << "\"";

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
  
  void http_session::write_http_response(unsigned int code, const char* json_content, size_t json_content_size)
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
    case HTTP_CODE_LENGTH_REQUIRED:
      reason = HTTP_REASON_LENGTH_REQUIRED;
      break;
    case HTTP_CODE_NOT_FOUND:
      reason = HTTP_REASON_NOT_FOUND;
      break;
    }
    
    std::ostream response(&_buffer_response);
    response << HTTP_VERSION_11 " " << code << " " << reason << HTTP_DELIMITER;
    if (json_content && json_content_size)
    {
      response << HTTP_HEADER_CONTENT_TYPE ": " HTTP_HEADER_CONTENT_TYPE_JSON HTTP_DELIMITER HTTP_HEADER_CONTENT_LENGTH ": " << json_content_size << HTTP_HEADER_END;
      response << json_content; // put content
    }
    else
    {
      response << HTTP_HEADER_CONTENT_TYPE ": " HTTP_HEADER_CONTENT_TYPE_TEXT HTTP_DELIMITER HTTP_HEADER_CONTENT_LENGTH ": " << sizeof(HTTP_BODY_NO_CONTENT) - 1 << HTTP_HEADER_END;
      response << HTTP_BODY_NO_CONTENT;
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
        // after move, _socket is ready to be used again as it is as freshly constructed !
      } else
      {
        LOG_ERROR << "error " << ec;
      }

      accept();
    });
  }

}
