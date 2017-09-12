#include <stdexcept>

#include "router.hpp"

using namespace http;
using tcp = boost::asio::ip::tcp;

/////////////////////////////////////
//Route
/////////////////////////////////////
Router::Route::Route(const char* path, router_handler fn) : callback(fn) {
    std::string regex_path("^");
    regex_path.append(path);

    const std::sregex_iterator iter_end;
    const std::regex component_regex(this->component_re);
    for (auto iter = std::sregex_iterator(regex_path.begin(), regex_path.end(), component_regex); iter != iter_end; ++iter) {
        //First group is name of component
        this->keys.emplace_back(iter->str(1));
    }

    this->path = std::regex(std::regex_replace(regex_path, component_regex, this->replace_re).append("\\/?$"));
}

std::optional<Router::matches> Router::Route::match(const char* uri) const {
    std::cmatch result;

    if (std::regex_match(uri, result, this->path)) {
        std::unordered_map<std::string, std::string> components;
        const auto len = result.size();

        //First element is full match so we start with first group instead
        for (auto idx = 1; idx < len; idx++) {
            components.emplace(this->keys[idx - 1], result[idx]);
        }

        return std::make_optional(components);
    }
    else {
        return std::nullopt;
    }
}

std::optional<Router::matches> Router::Route::match(boost::beast::string_view uri) const {
    const std::string target(uri);
    return this->match(target.c_str());
}

/////////////////////////////////////
//Router
/////////////////////////////////////
Router::Router() noexcept {}

Router& Router::add_route(Method method, const char* route, router_handler fn) {
    handler_map_t* handlers = this->map_method(static_cast<boost::beast::http::verb>(method));

    if (handlers == nullptr) throw std::range_error("Unknown method");

    handlers->emplace_back(route, fn);

    return *this;
}

bool Router::dispatch(const tcp::socket& socket, const dynamic_request& request, dynamic_response& response) const noexcept {
    const handler_map_t* handlers = this->map_method(request.method());

    if (handlers == nullptr) return false;

    for (const auto& handler : *handlers) {
        if (const auto matches = handler.match(request.target())) {
            handler.callback(Context{socket, request, response, *matches});
            return true;
        }
    }

    return false;
}

bool Router::is_method_used(boost::beast::http::verb method) const noexcept {
    const auto handlers = this->map_method(method);
    return handlers != nullptr && handlers->size() > 0;
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
