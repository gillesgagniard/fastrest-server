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

#include <fastrest/controller.hpp>
#include "http.hpp"

namespace fastrest
{
  controller::~controller()
  {

  }

  void controller::write_response(unsigned int code, const char* data, size_t data_size)
  {
    _http_session->write_http_response(code, data, data_size);
  }

  void controller::write_response_ok(const char* data, size_t data_size)
  {
    write_response(HTTP_CODE_OK, data, data_size);
  }

  void controller::write_response_not_found()
  {
    write_response(HTTP_CODE_NOT_FOUND);
  }

  void controller::write_response_bad_request()
  {
    write_response(HTTP_CODE_BAD_REQUEST);
  }

  const std::string& controller::get_method()
  {
    return _http_session->get_method();
  }

  const std::string& controller::get_uri()
  {
    return _http_session->get_uri();
  }
}
