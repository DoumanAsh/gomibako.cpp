#pragma once

#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <unordered_set>

#include <boost/beast/http/verb.hpp>

#include "beast_common.hpp"

namespace http {
    enum class Method {
        GET = boost::beast::http::verb::get,
        HEAD = boost::beast::http::verb::head,
        POST = boost::beast::http::verb::post,
        PUT = boost::beast::http::verb::put,
        DELETE_ = boost::beast::http::verb::delete_,
        CONNECT = boost::beast::http::verb::connect,
        OPTIONS = boost::beast::http::verb::options,
        TRACE = boost::beast::http::verb::trace
    };

    /**
     * HTTP Router class
     */
    class Router {
        ///Type of route's handler.
        typedef std::function<void(const dynamic_request&, dynamic_response&)> router_handler;

        public:
            ///Initializes router.
            Router() noexcept;

            /**
             * Adds route's handler.
             *
             * @param method Router's method.
             * @note Routes can be overridden.
             */
            Router& add_route(Method method, std::string&& route, router_handler fn);

            /**
             * Dispatches Beast HTTP request.
             *
             * @param request Beast HTTP request.
             * @param response Beast HTTP response.
             * @retval true If corresponding handler is found.
             * @retval false Otherwise.
             */
            bool dispatch(const dynamic_request& request, dynamic_response& response) const noexcept;

            /**
             * @return Whether method is used by @ref Router.
             */
            bool is_method_used(boost::beast::http::verb method) const noexcept;

        private:
            std::unordered_set<int> avail_methods;

            typedef std::vector<std::pair<std::string, router_handler>> handler_map_t;
            handler_map_t get_handlers;
            handler_map_t head_handlers;
            handler_map_t post_handlers;
            handler_map_t put_handlers;
            handler_map_t delete_handlers;
            handler_map_t options_handlers;
            handler_map_t trace_handlers;

            handler_map_t* map_method(boost::beast::http::verb method) noexcept;
            const handler_map_t* map_method(boost::beast::http::verb method) const noexcept;
    };
}
