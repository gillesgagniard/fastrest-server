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

#include "dispatcher.hpp"
#include <memory>
#include "logutils.hpp"
#include "http.hpp"
#include <fastrest/controller.hpp>

namespace fastrest
{
  void dispatcher::add(const std::string& uri, controller::create_controller_t controller_creator)
  {
    LOG_INFO << "register uri " << uri;
    _controllers.insert(std::make_pair(uri, controller_creator));
  }

  void dispatcher::execute(std::shared_ptr<http_session> session, std::string request_data)
  {
    LOG_INFO << "dispatch uri " << session->get_uri();
    auto hait = _controllers.find(session->get_uri());
    if (hait == _controllers.end())
    {
      LOG_ERROR << "unknown uri " << session->get_uri();
      session->write_http_response(HTTP_CODE_NOT_FOUND);
      return;
    }
    std::unique_ptr<controller> ha((*hait->second)());
    ha->_http_session = session;
    ha->_request_body = std::move(request_data);
    ha->execute();
  }
  
  dispatcher::~dispatcher()
  {
    
  }
}
