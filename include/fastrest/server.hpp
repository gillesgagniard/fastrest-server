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

#include <fastrest/controller.hpp>
#include <memory>

namespace fastrest
{
  class dispatcher;

  class server
  {
  public:
    server(unsigned int port);
    server(unsigned int port, unsigned int num_workers);
    ~server();

    template <class CONTROLLER>
    void add(const std::string& uri)
    {
      add(uri, &controller::create<CONTROLLER>);
    }

    void run();

  protected:
    void init();
    static unsigned int default_num_workers();

    void add(const std::string& uri, controller::create_controller_t controller_creator);

    unsigned int _num_workers;
    unsigned int _port;
    std::unique_ptr<dispatcher> _dispatcher; // hide implementation
  };
}
