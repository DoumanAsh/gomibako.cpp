#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http {
    using namespace boost::beast::http;
    constexpr size_t STATIC_BUFFER_MAX = 4096;

    ///Alias to beast::http::request
    typedef request<dynamic_body> dynamic_request;
    ///Alias to beast::http::response
    typedef response<dynamic_body> dynamic_response;
}
