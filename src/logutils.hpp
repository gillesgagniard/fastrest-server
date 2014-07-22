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

#pragma once

#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(fastrest_logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>);

#define LOG_DEBUG BOOST_LOG_SEV(fastrest_logger::get(), boost::log::trivial::debug)
#define LOG_INFO BOOST_LOG_SEV(fastrest_logger::get(), boost::log::trivial::info)
#define LOG_WARN BOOST_LOG_SEV(fastrest_logger::get(), boost::log::trivial::warn)
#define LOG_ERROR BOOST_LOG_SEV(fastrest_logger::get(), boost::log::trivial::error)
