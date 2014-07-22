/*
 * controller.cpp
 *
 *  Created on: 3 juin 2014
 *      Author: gilles
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

  const std::string& controller::get_method()
  {
    return _http_session->get_method();
  }

  const std::string& controller::get_uri()
  {
    return _http_session->get_uri();
  }
}
