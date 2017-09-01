#pragma once

#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <unordered_set>

#include <boost/beast/http/verb.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "beast_common.hpp"

namespace http {
    /**
     * Alias to boost::beast::http::verb
     */
    enum class Method {
        ///HTTP GET
        GET = boost::beast::http::verb::get,
        ///HTTP HEAD
        HEAD = boost::beast::http::verb::head,
        ///HTTP POST
        POST = boost::beast::http::verb::post,
        ///HTTP PUT
        PUT = boost::beast::http::verb::put,
        ///HTTP DELETE
        DELETE_ = boost::beast::http::verb::delete_,
        ///HTTP CONNECT
        CONNECT = boost::beast::http::verb::connect,
        ///HTTP OPTIONS
        OPTIONS = boost::beast::http::verb::options,
        ///HTTP TRACE
        TRACE = boost::beast::http::verb::trace
    };

    /**
     * HTTP Router class
     */
    class Router {
        public:
            /**
             * Context structure that contains beast's primitives.
             *
             * For obvious reasons only response can be modified.
             */
            struct Context {
                ///asio's socket can give access to useful information such as local/remote endpoint.
                const boost::asio::ip::tcp::socket& socket;
                ///boost::beast request
                const dynamic_request& request;
                ///boost::beast response into which handler writes stuff
                dynamic_response& response;

                //Since we store references here better to not let it get copied around
                //Of course nothing prevents user from storing particular reference somewhere,
                //but it is not my problem. I did all I could.
                Context(const Context& that) = delete;
                Context(Context&& other) noexcept = delete;
            };
            ///Type of route's handler.
            typedef std::function<void(Context&&)> router_handler;

            ///Initializes router.
            Router() noexcept;

            /**
             * Adds route's handler.
             *
             * @param method Router's method.
             * @param route String with URI path relative to host `<host:port></route>`
             * @param fn Handler for route.
             * @note Routes can be overridden.
             */
            Router& add_route(Method method, std::string&& route, router_handler fn);

            /**
             * Dispatches Beast HTTP request.
             *
             * @param socket Boost asio's socket.
             * @param request Beast HTTP request.
             * @param response Beast HTTP response.
             * @retval true If corresponding handler is found.
             * @retval false Otherwise.
             */
            bool dispatch(const boost::asio::ip::tcp::socket& socket, const dynamic_request& request, dynamic_response& response) const noexcept;

            /**
             * @return Whether method is used by @ref Router.
             */
            bool is_method_used(boost::beast::http::verb method) const noexcept;

        private:
            /**
             * Stores HTTP methods which were inserted into routed.
             *
             * It may be useful to pre-check if method is available at all
             * instead of iterating through all available methods.
             */
            std::unordered_set<int> avail_methods;

            ///Router's handler map to UR paths type
            typedef std::vector<std::pair<std::string, router_handler>> handler_map_t;
            ///Map of GET handlers
            handler_map_t get_handlers;
            ///Map of HEAD handlers
            handler_map_t head_handlers;
            ///Map of POST handlers
            handler_map_t post_handlers;
            ///Map of PUT handlers
            handler_map_t put_handlers;
            ///Map of DELETE handlers
            handler_map_t delete_handlers;
            ///Map of OPTIONS handlers
            handler_map_t options_handlers;
            ///Map of TRACE handlers
            handler_map_t trace_handlers;

            ///Retrieves map according to method
            handler_map_t* map_method(boost::beast::http::verb method) noexcept;
            ///Retrieves const map according to method
            const handler_map_t* map_method(boost::beast::http::verb method) const noexcept;
    };
}
