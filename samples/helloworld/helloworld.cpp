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
#include <sstream>

class helloworld_controller : public fastrest::controller
{
  void execute()
  {
    std::ostringstream out;
    out << "Hello World ! method=" << get_method() << " uri=" << get_uri();
    write_response(200, out.str().c_str(), out.str().length());
  }
};

int main()
{
  fastrest::server my_server(6666);
  my_server.add<helloworld_controller>("/hello");
  my_server.run();
}
