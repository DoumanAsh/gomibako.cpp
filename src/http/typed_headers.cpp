#include <string_view>

#include "typed_headers.hpp"

using namespace http;

ETag::ETag(std::string tag, bool weak=false) : tag(tag), weak(weak) {};

std::optional<ETag> ETag::parse(const std::string& raw_header) {
    constexpr const char* QUOTE = "\"";

    bool weak = false;

    const auto left = raw_header.find(QUOTE);
    if (left == std::string::npos) return std::nullopt;
    else if (left > 0) weak = raw_header.compare(0, left, "W/") == 0;

    const auto right = raw_header.find(QUOTE, left);
    if (right == std::string::npos) return std::nullopt;

    return ETag(raw_header.substr(left, right), weak);
}

boost::beast::http::field ETag::name() const {
    return boost::beast::http::field::etag;
}

boost::beast::string_param&& ETag::value() const {
    return std::move(boost::beast::string_param(this->weak ? "W/\"" : "\"", this->tag, "\""));
}
