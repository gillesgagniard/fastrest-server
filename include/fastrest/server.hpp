/*
 * server.hpp
 *
 *  Created on: 2 juin 2014
 *      Author: gilles
 */

#pragma once

namespace fastrest
{
  class server
  {
  public:
    server(unsigned int port);
    server(unsigned int port, unsigned int num_workers);

    void run();

  protected:
    void init();
    static unsigned int default_num_workers();

    unsigned int _num_workers;
    unsigned int _port;
  };
}
