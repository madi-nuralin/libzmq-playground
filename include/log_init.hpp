#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void log_init()
{
    logging::add_console_log(std::clog, keywords::format = "[%Severity%]: %Message%");
    logging::add_common_attributes();
}
