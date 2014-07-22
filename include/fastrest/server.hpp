/*
 * server.hpp
 *
 *  Created on: 2 juin 2014
 *      Author: gilles
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
