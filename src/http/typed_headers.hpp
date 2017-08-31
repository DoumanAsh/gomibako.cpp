#pragma once

#include <string>
#include <optional>

#include <boost/beast/http/field.hpp>
#include <boost/beast/core/string_param.hpp>

namespace http {
    /**
     * Typed header interface.
     *
     * It provides way to serialize and de-serialize http header.
     */
    class TypedHeader {
        public:
            ///@returns HTTP header name
            virtual boost::beast::http::field name() const = 0;
            ///@returns HTTP header value
            virtual boost::beast::string_param&& value() const = 0;
    };


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
            boost::beast::http::field name() const override;
            boost::beast::string_param&& value() const override;
    };
}
