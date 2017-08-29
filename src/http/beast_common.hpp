#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http {
    using namespace boost::beast::http;

    typedef request<dynamic_body> dynamic_request;
    typedef response<dynamic_body> dynamic_response;
}
