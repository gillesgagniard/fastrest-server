/*
 * Copyright (c) 2014 Gilles Gagniard <gilles@gagniard.org>
 *
 * This file is part of fastrest-server.
 *
 * fastrest-server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * fastrest-server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fastrest-server.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fastrest/controller.hpp>
#include "http.hpp"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace fastrest
{
  controller::~controller()
  {

  }

  void controller::write_response_ok(const char* data, size_t data_size)
  {
    _http_session->write_http_response(HTTP_CODE_OK, http_session::CONTENT_TYPE_TEXT, data, data_size);
  }

  void controller::write_response_ok(const rapidjson::Document& data)
  {
    rapidjson::StringBuffer reply;
    rapidjson::Writer<rapidjson::StringBuffer> reply_writer(reply);
    data.Accept(reply_writer);
    _http_session->write_http_response(HTTP_CODE_OK, http_session::CONTENT_TYPE_JSON, reply.GetString(), reply.Size());
  }

  void controller::write_response_not_found()
  {
    _http_session->write_http_response(HTTP_CODE_NOT_FOUND);
  }

  void controller::write_response_bad_request()
  {
    _http_session->write_http_response(HTTP_CODE_BAD_REQUEST);
  }

  void controller::write_response_method_not_allowed()
  {
    _http_session->write_http_response(HTTP_CODE_METHOD_NOT_ALLOWED);
  }

  const std::string& controller::get_method() const
  {
    return _http_session->get_method();
  }

  const std::string& controller::get_uri() const
  {
    return _http_session->get_uri();
  }

  bool controller::is_body_json() const
  {
    return _http_session->get_content_type() == http_session::CONTENT_TYPE_JSON;
  }

  const rapidjson::Document& controller::get_body_document()
  {
    if (!is_body_json())
    {
      throw controller_bad_content_type();
    }
    if (_request_body_document.IsNull())
    {
      // TODO use ParseInsitu instead ?
      _request_body_document.Parse<rapidjson::kParseDefaultFlags>(get_body().c_str());
    }
    return _request_body_document;
  }
}
