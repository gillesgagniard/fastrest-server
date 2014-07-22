/*
 * controller.hpp
 *
 *  Created on: 2 juin 2014
 *      Author: gilles
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

    void write_response(unsigned int code, const char* data, size_t data_size);

  protected:


    typedef controller*(*create_controller_t)();

    template <class CONTROLLER>
    static controller* create()
    {
      return new CONTROLLER;
    }

  private:
    std::string _request_body;
    std::shared_ptr<http_session> _http_session;
  };
}
