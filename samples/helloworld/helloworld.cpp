/*
 * helloworld.cpp
 *
 *  Created on: 2 juin 2014
 *      Author: gilles
 */

#include <fastrest/server.hpp>
#include <fastrest/controller.hpp>

int main()
{
  fastrest::server my_server(6666);
  my_server.run();
}
