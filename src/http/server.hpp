#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include "../config/config.hpp"

namespace http {
    class Server {
        public:
            /**
             * Initializes Server from config.
             */
            Server(config::Config&& config) noexcept;
            /**
             * Starts HTTP Server
             */
            void start();

        private:
            config::Config config;
    };
}
