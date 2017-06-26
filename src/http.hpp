#pragma once

extern "C" {
#include "mongoose.h"
}

#include <utility>
#include <vector>

#include "http/message.hpp"

namespace http {
    typedef void (*http_route_hander)(Response&, const Request&);
    typedef std::pair<const char*, http_route_hander> http_route;

    /**
     * Class wrapper over mongoose http server
     */
    class Server {
        public:
            /**
             * Initializes server with default addr.
             */
            Server();
            /**
             * Initializes server with provided address.
             *
             * @param[in] addr Address on which to start server. Its format is `[host:]<port>`
             */
            explicit Server(const char* addr);
            /** Terminates server */
            ~Server();

            /**
             * Starts-up server indefinitely.
             */
            void start();

            /**
             * Sets sleep time
             * @param[in] ms Maximum sleep for server. Default 1000
             */
            void set_sleep_time(int ms);

            /**
             * Sets directory to serve.
             * @param[in] dir Valid path to a directory.
             */
            void set_serve_dir(const char *dir);

            /**
             * Sets directory to serve.
             * @param[in] dir Valid path to a directory.
             * @param[in] is_listing Whether to enable directory listing. On by default.
             */
            void set_serve_dir(const char *dir, bool is_listing);

            /**
             * Retrieves mongoose HTTP options.
             * @return pointer to options structure.
             */
            const struct mg_serve_http_opts* get_http_options() const;

            /**
             * Adds route to server.
             * @param[in] path Path to call handler on.
             * @param[in] handler Function to invoke on match.
             */
            void add_route(const char* path, http_route_hander handler);

            /**
             * @return Registered routes.
             */
            const std::vector<http_route>& get_routes() const;

            /**
             * Adds route to server that is invoked when no other matched.
             *
             * Suitable for 404.
             * @param[in] handler Function to invoke.
             */
            void add_default_route(http_route_hander handler);

            /**
             * @return Default router function.
             */
            const http_route_hander get_default_route() const;

        private:
            struct mg_mgr manager;
            struct mg_connection *conn;
            struct mg_serve_http_opts http_options;

            int max_sleep;
            const char *serve_dir;
            std::vector<http_route> routes;
            http_route_hander default_route;
    };
}
