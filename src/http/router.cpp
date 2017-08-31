#include <stdexcept>

#include "router.hpp"

using namespace http;
using tcp = boost::asio::ip::tcp;

Router::Router() noexcept {
}

Router& Router::add_route(Method method, std::string&& route, router_handler fn) {
    handler_map_t* handlers = this->map_method(static_cast<boost::beast::http::verb>(method));

    if (handlers == nullptr) throw std::range_error("Unknown method");

    this->avail_methods.emplace(static_cast<int>(method));
    handlers->emplace_back(route, fn);

    return *this;
}

bool Router::dispatch(const tcp::socket& socket, const dynamic_request& request, dynamic_response& response) const noexcept {
    const handler_map_t* handlers = this->map_method(request.method());

    if (handlers == nullptr) return false;

    for (const auto& handler : *handlers) {
        if (handler.first == request.target()) {
            handler.second(Context{socket, request, response});
            return true;
        }
    }

    return false;
}

bool Router::is_method_used(boost::beast::http::verb method) const noexcept {
    return this->avail_methods.count(static_cast<int>(method)) == 1;
}


Router::handler_map_t* Router::map_method(boost::beast::http::verb method) noexcept {
    switch (method) {
        case Method::GET: return &this->get_handlers;
        case Method::HEAD: return &this->head_handlers;
        case Method::POST: return &this->post_handlers;
        case Method::PUT: return &this->put_handlers;
        case Method::DELETE_: return &this->delete_handlers;
        case Method::OPTIONS: return &this->options_handlers;
        case Method::TRACE: return &this->trace_handlers;
        default: return nullptr;
    }
}

const Router::handler_map_t* Router::map_method(boost::beast::http::verb method) const noexcept {
    //Hack around code duplication.
    //Should be safe?
    return const_cast<Router*>(this)->map_method(method);
}
