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
  public:
  protected:
    std::string _request_data;
  private:
    std::shared_ptr<http_session> _http_session;
  };
}
