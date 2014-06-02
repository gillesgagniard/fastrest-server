/* 
 * File:   http_action.cpp
 * Author: gilles
 * 
 * Created on 11 mars 2014, 21:26
 */

#include "http_action.hpp"
#include <memory>
#include "logutils.hpp"
#include "http_server.hpp"

namespace fastrest
{
  void http_action::execute(std::shared_ptr<http_session> session, std::string request_data)
  {
    LOG_INFO << "execute action " << session->get_uri();
    auto hait = _actions.find(session->get_uri());
    if (hait == _actions.end())
    {
      LOG_ERROR << "unknown action " << session->get_uri();
      session->write_http_response(HTTP_CODE_NOT_FOUND);
    }
    std::unique_ptr<http_action> ha((*hait->second)());
    ha->_http_session = session;
    ha->_request_data = std::move(request_data);
    ha->execute();
  }
  
  http_action::~http_action()
  {
    
  }
  
  std::map<std::string, http_action::create_http_action_t> http_action::_actions;
}
