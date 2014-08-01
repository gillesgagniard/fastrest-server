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
#include <fastrest/controller.hpp>

class helloworld_controller : public fastrest::controller
{
  void execute()
  {
    // only accept POST with a JSON document
    if (get_method() != "POST" || !is_body_json())
    {
      write_response_method_not_allowed();
      return;
    }

    // get fellow name
    const rapidjson::Document& body = get_body_document();
    std::string name;
    if (body.IsObject() && body.HasMember("name"))
    {
      name = body["name"].GetString();
    }
    else
    {
      write_response_bad_request();
      return;
    }

    // reply to him politely
    rapidjson::Document reply;
    reply.SetObject();
    reply.AddMember("hello", name.c_str(), reply.GetAllocator());
    write_response_ok(reply);
  }
};

int main()
{
  fastrest::server my_server(6666);
  my_server.add<helloworld_controller>("/hello");
  my_server.run();
}
