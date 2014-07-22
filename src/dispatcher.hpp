/* 
 * File:   dispatcher.hpp
 * Author: gilles
 *
 * Created on 11 mars 2014, 21:26
 */

#pragma once

#include <string>
#include <map>
#include <exception>
#include <memory>
#include <fastrest/controller.hpp>

namespace fastrest
{
  
  class http_session;
  class controller;

  class dispatcher
  {
  public:
    void execute(std::shared_ptr<http_session> session, std::string request_data);
    
    void add(const std::string& uri, controller::create_controller_t controller_creator);

    virtual ~dispatcher();
    
  protected:
    std::map<std::string, controller::create_controller_t> _controllers; // map uri -> controller creator
  };

}
