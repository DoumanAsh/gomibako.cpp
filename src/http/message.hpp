#pragma once

extern "C" {
#include "mongoose.h"
}

#include <optional>
#include <utility>
#include <iostream>
#include <string>
#include <cstdint>

namespace http {
    enum class Method {
        GET,
        HEAD,
        POST,
        PUT,
        DEL,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH
    };

    const char* method_to(Method);

    ///Finishes headers part by appending newline separator.
    constexpr char* header_end = "\r\n";

    typedef std::pair<std::string, std::string> header;

    class Headers {
        private:
            size_t idx;
            struct mg_str* names;
            struct mg_str* values;

        public:
            Headers(struct mg_str* names, struct mg_str* values);
            Headers(const Headers&);

            /**
             * Retrieves header by name.
             * @returns optional with header, if it is present.
             */
            std::optional<header> get(const char* name);
            ///Returns next element.
            std::optional<header> next();
            ///Returns previous element.
            std::optional<header> prev();
    };

    class Request {
        public:
            ///Initializes request by wrapping mongoose http_message.
            Request(struct http_message*, struct mg_connection*);

            ///Retrieves request's body.
            const char* body() const;

            ///Retrieves request's method: GET, PUT and etc.
            Method method() const;

            ///Retrieves URI on which request came.
            const char* uri() const;

            ///Retrieves request's query string.
            const char* query_string() const;

            ///@returns Content of header.
            ///@param[in] name Name of header to retrieve.
            ///@retval NULL if no such header.
            const char* header(const char* name) const;

            ///@returns Headers pseudo iterator.
            Headers headers() const;

            ///Retrieves IP from which request came.
            std::string remote_ip() const;
        private:
            struct http_message *inner;
            struct mg_connection *conn;
    };

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

            ///Destructor that sets message to go into network
            virtual ~Response();

            friend std::istream& operator>>(std::istream&, Response&);

            /**
             * Initiates message sending by writing status code and length header.
             *
             * @return this.
             */
            Response& start();

            ///Response status code. Default 500.
            int status_code;
            ///Response length. Default 0.
            int64_t len;

        private:
            struct mg_connection *conn;
    };

}
