/* 
 * File:   logutils.hpp
 * Author: gilles
 *
 * Created on 19 décembre 2013, 18:17
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
