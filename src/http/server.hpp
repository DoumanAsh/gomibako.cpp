#pragma once

#include "router.hpp"
#include "../config/config.hpp"

namespace http {
    class Server {
        public:
            /**
             * Initializes Server from config.
             */
            Server(config::Config&& config, Router&& router) noexcept;
            /**
             * Starts HTTP Server
             */
            void start();

        private:
            config::Config config;
            Router router;
    };
}
