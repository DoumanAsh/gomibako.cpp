#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http {
    using namespace boost::beast::http;
    constexpr size_t STATIC_BUFFER_MAX = 4096;

    ///Alias to beast::http::request
    using dynamic_request = request<dynamic_body>;
    ///Alias to beast::http::response
    using dynamic_response = response<dynamic_body>;
}
