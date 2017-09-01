#pragma once

#include <boost/system/error_code.hpp>

/**
 * Misc. utilities module.
 */
namespace utils {
    /**
     * Logs Boost error code.
     * @param[in] error Boost Error Code
     */
    void log_error(const boost::system::error_code& error);
    /**
     * Logs Boost error code.
     * @param[in] error Boost Error Code
     * @param[in] prefix Prefix of error message.
     */
    void log_error(const boost::system::error_code& error, const char* prefix);
}
