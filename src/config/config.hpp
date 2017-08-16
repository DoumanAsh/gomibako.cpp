#pragma once

#include <thread>

#include <boost/asio/ip/tcp.hpp>

namespace config {
    /**
     * Application config.
     */
    class Config {
        public:
            ///Number of available CPUs
            unsigned int cpu_num = std::thread::hardware_concurrency();
            ///IP Address to use
            boost::asio::ip::address address = boost::asio::ip::address::from_string("0.0.0.0");
            ///Port to use
            unsigned short port = 3333;
    };
}
