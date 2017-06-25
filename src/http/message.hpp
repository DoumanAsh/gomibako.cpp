#pragma once

extern "C" {
#include "mongoose.h"
}

#include <iostream>
#include <cstdint>

namespace http {
    ///Finishes headers part by appending newline separator.
    constexpr char* header_end = "\r\n";

    class Response {
        public:
            ///Initializes default message with HTTP code 500.
            Response(struct mg_connection *conn);
            /**
             * Initializes message.
             * @param[in] status HTTP code to assign.
             */
            explicit Response(struct mg_connection *conn, int status);

            /**
             * Initializes message.
             * @param[in] status HTTP code to assign.
             * @param[in] len Payload's len. It goes to Content-Length header.
             */
            explicit Response(struct mg_connection *conn, int status, int64_t len);

            friend std::istream& operator>>(std::istream&, Response&);

            /**
             * Initiates message sending by writing status code and length header.
             *
             * @return this.
             */
            Response& start();

            ///Finish building message and send it out.
            void end();

            ///Response status code. Default 500.
            int status_code;
            ///Response length. Default 0.
            int64_t len;

        private:
            struct mg_connection *conn;
    };

}
