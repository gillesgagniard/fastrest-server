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
