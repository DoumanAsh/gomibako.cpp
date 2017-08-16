#include <iostream>

#include "format.hpp"

using namespace utils;

void utils::log_error(const boost::system::error_code& error) {
    std::cerr << error.message() << "\n";
}

void utils::log_error(const boost::system::error_code& error, const char* prefix) {
    std::cerr << prefix << ": " << error.message() << "\n";
}
