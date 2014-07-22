/* 
 * File:   server.cpp
 * Author: gilles
 * 
 * Created on 18 septembre 2013, 00:22
 */

#include <fastrest/server.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/thread/thread.hpp>

#include "logutils.hpp"
#include "http.hpp"
#include "threadpool.hpp"
#include "dispatcher.hpp"

namespace fastrest
{
  server::server(unsigned int port) : server(port, default_num_workers())
  {
  }

  server::server(unsigned int port, unsigned int num_workers) :
      _port(port), _num_workers(num_workers), _dispatcher(new dispatcher)
  {
    init();
  }

  server::~server()
  {
  }

  unsigned int server::default_num_workers()
  {
    unsigned int default_workers = std::max(1, (int)boost::thread::hardware_concurrency() - 2);
    LOG_DEBUG << "default worker threads : " << default_workers;
    return default_workers;
  }

  void server::init()
  {
    // init logging
    namespace lgs = boost::log;
    lgs::add_common_attributes();
    lgs::register_simple_formatter_factory< lgs::trivial::severity_level, char >("Severity");
  }

  void server::run()
  {
    threadpool tpool(_num_workers);
    namespace io = boost::asio;
    io::io_service io_service;
    io::ip::tcp::endpoint endpoint(io::ip::tcp::v4(), _port);
    http_server s(io_service, endpoint, tpool, *_dispatcher);
    io_service.run();
  }

  void server::add(const std::string& uri, controller::create_controller_t controller_creator)
  {
    _dispatcher->add(uri, controller_creator);
  }
}
