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

#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <utility>
#include <memory>
#include <sstream>
#include "dispatcher.hpp"

#define HTTP_CODE_OK 200
#define HTTP_REASON_OK "OK"

#define HTTP_CODE_BAD_REQUEST 400
#define HTTP_REASON_BAD_REQUEST "Bad Request"

#define HTTP_CODE_NOT_FOUND 404
#define HTTP_REASON_NOT_FOUND "Not Found"

#define HTTP_CODE_METHOD_NOT_ALLOWED 405
#define HTTP_REASON_METHOD_NOT_ALLOWED "Method Not Allowed"

#define HTTP_CODE_LENGTH_REQUIRED 411
#define HTTP_REASON_LENGTH_REQUIRED "Length Required"

#define HTTP_DELIMITER "\r\n"
#define HTTP_HEADER_END "\r\n\r\n"
#define HTTP_VERSION_10 "HTTP/1.0"
#define HTTP_VERSION_11 "HTTP/1.1"
#define HTTP_HEADER_CONTENT_TYPE "Content-Type"
#define HTTP_HEADER_CONTENT_TYPE_JSON "application/json"
#define HTTP_HEADER_CONTENT_TYPE_TEXT "text/plain"
#define HTTP_HEADER_CONTENT_TYPE_XML "application/xml"
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_CONNECTION "Connection"
#define HTTP_HEADER_CONNECTION_CLOSE "close"

#define HTTP_BODY_NO_CONTENT "No Content"

namespace fastrest
{
  
  class threadpool;

  class http_session : public std::enable_shared_from_this<http_session>
  {
  public:

    enum ContentType_t
    {
      CONTENT_TYPE_TEXT = 1000,
      CONTENT_TYPE_JSON = 1001,
      CONTENT_TYPE_XML = 1002,
      CONTENT_TYPE_OTHER = 2000
    };

    http_session(boost::asio::ip::tcp::socket s, threadpool& tpool, dispatcher& dispatcher);
    void start();
    
    void write_http_response(unsigned int code, ContentType_t content_type = CONTENT_TYPE_OTHER, const char* data = NULL, size_t data_size = 0);
    
    inline const std::string& get_method()
    {
      return _method;
    }

    inline const std::string& get_uri()
    {
      return _uri;
    }

    inline ContentType_t get_content_type()
    {
      return _content_type;
    }

  protected:

    void read_http_header();
    void read_http_body(size_t body_length);

    boost::asio::ip::tcp::socket _socket;
    threadpool& _tpool;
    dispatcher& _dispatcher;
    boost::asio::streambuf _buffer_request;
    boost::asio::streambuf _buffer_response;
    
    std::string _method;
    std::string _uri;
    std::string _version;
    ContentType_t _content_type;
    bool _connection_close;
  } ;

  class http_server
  {
  public:

    http_server(boost::asio::io_service& io_service, boost::asio::ip::tcp::endpoint& endpoint, threadpool& tpool, dispatcher& dispatcher);

  protected:

    void accept();

    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;
    threadpool& _tpool;
    dispatcher& _dispatcher;
  } ;

}
