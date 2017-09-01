#include <string_view>
#include <sstream>
#include <iostream>

#include "typed_headers.hpp"

using namespace http::header;

//ETag
ETag::ETag(const std::string& tag, bool weak=false) : tag(tag), weak(weak) {};

std::optional<ETag> ETag::parse(const std::string& raw_header) {
    constexpr const char* QUOTE = "\"";

    bool weak = false;

    const auto left = raw_header.find(QUOTE);
    if (left == std::string::npos) return std::nullopt;
    else if (left > 0) weak = raw_header.compare(0, left, "W/") == 0;

    const auto right = raw_header.find_last_of(QUOTE);
    if (right == std::string::npos) return std::nullopt;

    return ETag(raw_header.substr(left + 1, right - left - 1), weak);
}

boost::beast::http::field ETag::name() const {
    return boost::beast::http::field::etag;
}

std::string ETag::value() const {
    std::stringstream result;

    result << (this->weak ? "W/\"" : "\"")
           << this->tag
           << "\"";

    return result.str();
}

//ContentType
Server::Server(const char* name) noexcept : server(name) {};

boost::beast::http::field Server::name() const {
    return boost::beast::http::field::server;
}

std::string Server::value() const {
    return this->server;
}

//ContentType
ContentType::ContentType(const std::string& type) : type(type) {};
ContentType::ContentType(const char* type) : type(type) {};

ContentType ContentType::json() { return ContentType("application/json"); }
ContentType ContentType::plain_text() { return ContentType("text/plain"); }
ContentType ContentType::html() { return ContentType("text/html"); }
ContentType ContentType::form_url_enc() { return ContentType("application/www-form-url-encoded"); }
ContentType ContentType::jpeg() { return ContentType("image/jpeg"); }
ContentType ContentType::png() { return ContentType("image/png"); }
ContentType ContentType::octet_stream() { return ContentType("application/octet_stream"); }

boost::beast::http::field ContentType::name() const {
    return boost::beast::http::field::content_type;
}

std::string ContentType::value() const {
    return this->type;
}
