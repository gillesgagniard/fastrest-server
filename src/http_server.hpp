/* 
 * File:   http_server.hpp
 * Author: gilles
 *
 * Created on 9 mars 2014, 23:40
 */

#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <utility>
#include <memory>
#include <sstream>

#define HTTP_CODE_OK 200
#define HTTP_REASON_OK "OK"

#define HTTP_CODE_BAD_REQUEST 400
#define HTTP_REASON_BAD_REQUEST "Bad Request"

#define HTTP_CODE_NOT_FOUND 404
#define HTTP_REASON_NOT_FOUND "Not Found"

#define HTTP_CODE_LENGTH_REQUIRED 411
#define HTTP_REASON_LENGTH_REQUIRED "Length Required"

#define HTTP_HEADER_END "\r\n\r\n"
#define HTTP_VERSION_10 "HTTP/1.0"
#define HTTP_VERSION_11 "HTTP/1.1"
#define HTTP_HEADER_CONTENT_TYPE "Content-Type"
#define HTTP_HEADER_CONTENT_TYPE_JSON "application/json"
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_CONNECTION "Connection"
#define HTTP_HEADER_CONNECTION_CLOSE "close"

namespace fastrest
{
  
  class threadpool;

  class http_session : public std::enable_shared_from_this<http_session>
  {
  public:

    http_session(boost::asio::ip::tcp::socket s, threadpool& tpool);
    void start();
    
    void write_http_response(unsigned int code, const char* json_content = NULL, size_t json_content_size = 0);
    
    const std::string& get_uri()
    {
      return _uri;
    }

  protected:

    void read_http_header();
    void read_http_body(size_t body_length);

    boost::asio::ip::tcp::socket _socket;
    threadpool& _tpool;
    boost::asio::streambuf _buffer_request;
    boost::asio::streambuf _buffer_response;
    
    std::string _method;
    std::string _uri;
    std::string _version;
    bool _connection_close;
  } ;

  class http_server
  {
  public:

    http_server(boost::asio::io_service& io_service, boost::asio::ip::tcp::endpoint& endpoint, threadpool& tpool);

  protected:

    void accept();

    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;
    threadpool& _tpool;
  } ;

}