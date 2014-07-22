/* 
 * File:   dispatcher.cpp
 * Author: gilles
 * 
 * Created on 11 mars 2014, 21:26
 */

#include "dispatcher.hpp"
#include <memory>
#include "logutils.hpp"
#include "http.hpp"
#include <fastrest/controller.hpp>

namespace fastrest
{
  void dispatcher::add(const std::string& uri, controller::create_controller_t controller_creator)
  {
    LOG_INFO << "register uri " << uri;
    _controllers.insert(std::make_pair(uri, controller_creator));
  }

  void dispatcher::execute(std::shared_ptr<http_session> session, std::string request_data)
  {
    LOG_INFO << "dispatch uri " << session->get_uri();
    auto hait = _controllers.find(session->get_uri());
    if (hait == _controllers.end())
    {
      LOG_ERROR << "unknown uri " << session->get_uri();
      session->write_http_response(HTTP_CODE_NOT_FOUND);
      return;
    }
    std::unique_ptr<controller> ha((*hait->second)());
    ha->_http_session = session;
    ha->_request_body = std::move(request_data);
    ha->execute();
  }
  
  dispatcher::~dispatcher()
  {
    
  }
}
