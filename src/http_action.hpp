/* 
 * File:   http_action.hpp
 * Author: gilles
 *
 * Created on 11 mars 2014, 21:26
 */

#pragma once

#include <string>
#include <map>
#include <exception>
#include <memory>

namespace fastrest
{
  
  class http_session;

  class http_action
  {
  public:
    static void execute(std::shared_ptr<http_session> session, std::string request_data);
    
    template <class HTTP_ACTION>
    static void add()
    {
      _actions.insert(std::make_pair(HTTP_ACTION::uri, &create<HTTP_ACTION>));
    }
    
    virtual ~http_action();
    
  protected:
    typedef http_action*(*create_http_action_t)();
    static std::map<std::string, create_http_action_t> _actions; // map uri -> http_action creator
    
    template <class HTTP_ACTION>
    static http_action* create()
    {
      return new HTTP_ACTION;
    }
    
    virtual void execute() =0;
    
    std::shared_ptr<http_session> _http_session;
    std::string _request_data;
  };
  
  class http_action_unknown : public std::exception
  {
  public:
  };

}
