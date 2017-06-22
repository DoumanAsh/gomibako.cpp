#pragma once

extern "C" {
#include "mongoose.h"
}

namespace http {
    /**
     * Event handler function type
     */
    typedef void (*event_handler)(struct mg_connection*, int, void*);

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

        private:
        struct mg_mgr manager;
        struct mg_connection *conn;

        int max_sleep;
        const char *serve_dir;
    };
}
