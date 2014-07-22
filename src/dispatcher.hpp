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

#include <string>
#include <map>
#include <exception>
#include <memory>
#include <fastrest/controller.hpp>

namespace fastrest
{
  
  class http_session;
  class controller;

  class dispatcher
  {
  public:
    void execute(std::shared_ptr<http_session> session, std::string request_data);
    
    void add(const std::string& uri, controller::create_controller_t controller_creator);

    virtual ~dispatcher();
    
  protected:
    std::map<std::string, controller::create_controller_t> _controllers; // map uri -> controller creator
  };

}
