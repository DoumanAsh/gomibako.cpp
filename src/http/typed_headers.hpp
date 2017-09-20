#pragma once

#include <string>
#include <optional>
#include <type_traits>

#include <boost/beast/core/string.hpp>
#include <boost/beast/http/field.hpp>

namespace http {
    /**
     * HTTP Typed headers module.
     *
     * It provides guarantees of headers correctness and utilities to work with HTTP headers.
     */
    namespace header {
        /**
         * Sets typed header into beast response.
         *
         * @param message Beast's HTTP message.
         * @param header Object that implements @ref TypedHeader.
         */
        template<class M, class H>
        void set(M& message, H&& header) {
            static_assert(std::is_base_of<TypedHeader, H>::value, "Provided header doesn't implement TypedHeader interface");
            message.set(header.name(), header.value());
        }

        /**
         * Typed header interface.
         *
         * It provides way to serialize and de-serialize http header.
         */
        class TypedHeader {
            public:
                ///@returns HTTP header name
                virtual boost::beast::string_view name() const = 0;
                ///@returns HTTP header value
                virtual std::string value() const = 0;
        };

        /**
         * ETag header, defined in RFC7232.
         */
        class ETag: TypedHeader {
            public:
                /**
                 * Creates ETag instance.
                 *
                 * @param tag Tag's value.
                 * @param weak Whether ETag is weak or not. Default is weak.
                 */
                ETag(std::string tag, bool weak);
                /**
                 * Parses raw HTTP header ETag and constructs @ref ETag.
                 *
                 * @param raw_header HTTP header ETag string.
                 * @returns Instance of @ref ETag.
                 */
                static std::optional<ETag> parse(const std::string& raw_header);

                ///Weakness flag.
                bool weak;
                ///ETag's value.
                std::string tag;

                //TypedHeader interface
                boost::beast::string_view name() const override;
                std::string value() const override;
        };

        /**
         * Server header, defined in RFC7231.
         */
        class Server: TypedHeader {
            public:
                /**
                 * Creates @ref Server.
                 *
                 * @param name Server's header value.
                 */
                explicit Server(const char* name) noexcept;

                ///Header's value
                const char* server;

                //TypedHeader interface
                boost::beast::string_view name() const override;
                std::string value() const override;
        };

        /**
         * Content-Type header, defined in RFC7231.
         */
        class ContentType: TypedHeader {
            public:
                /**
                 * Creates @ref ContentType instance from raw string.
                 *
                 * @param type ContentType's string.
                 */
                explicit ContentType(std::string type);
                explicit ContentType(const char* type);

                ///Creates ContentType for JSON.
                static ContentType json();
                ///Creates ContentType for plain text.
                static ContentType plain_text();
                ///Creates ContentType for HTML.
                static ContentType html();
                ///Creates ContentType for web forms content.
                static ContentType form_url_enc();
                ///Creates ContentType for JPEG.
                static ContentType jpeg();
                ///Creates ContentType for PNG.
                static ContentType png();
                ///Creates ContentType for octet stream.
                static ContentType octet_stream();

                ///ContentType's text.
                std::string type;

                //TypedHeader interface
                boost::beast::string_view name() const override;
                std::string value() const override;
        };

        /**
         * Content-Type header, defined in RFC7231.
         */
        class UserAgent: TypedHeader {
            public:
                /**
                 * Creates @ref UserAgent.
                 *
                 * @param name UserAgent's header value.
                 */
                explicit UserAgent(const char* name) noexcept;

                ///UserAgent's value
                const char* agent;

                //TypedHeader interface
                boost::beast::string_view name() const override;
                std::string value() const override;
        };

    }
}
