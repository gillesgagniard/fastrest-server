/*
 * helloworld.cpp
 *
 *  Created on: 2 juin 2014
 *      Author: gilles
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
