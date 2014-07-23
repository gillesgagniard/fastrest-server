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
#include <memory>

namespace fastrest
{
  class http_session;

  class controller
  {
    friend class dispatcher;
    friend class server;

  public:
    virtual void execute() =0;
    virtual ~controller();

    const std::string& get_method();
    const std::string& get_uri();
    inline const std::string& get_body()
    {
      return _request_body;
    }

    void write_response_ok(const char* data = NULL, size_t data_size = 0);
    void write_response_not_found();
    void write_response_bad_request();

  protected:
    typedef controller*(*create_controller_t)();

    template <class CONTROLLER>
    static controller* create()
    {
      return new CONTROLLER;
    }

  private:
    void write_response(unsigned int code, const char* data = NULL, size_t data_size = 0);

    std::string _request_body;
    std::shared_ptr<http_session> _http_session;
  };
}
