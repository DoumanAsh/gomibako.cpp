#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "http/server.hpp"
#include "http/typed_headers.hpp"

using tcp = boost::asio::ip::tcp;

/**
 * Root page
 */
void hello_wolrd(http::Router::Context&& ctx) {
    ctx.response.result(http::status::ok);
    http::header::set(ctx.response, http::header::ContentType::html());
    boost::beast::ostream(ctx.response.body) << "<h1>Hello world</h1>\n";
}

/**
 * Handler for `/ip` route
 */
void handle_ip(http::Router::Context&& ctx) {
    boost::property_tree::ptree content;
    content.put("ip", ctx.socket.remote_endpoint().address().to_string());

    ctx.response.result(http::status::ok);
    http::header::set(ctx.response, http::header::ContentType::json());
    auto body_stream = boost::beast::ostream(ctx.response.body);
    boost::property_tree::write_json(body_stream, content, false);
}

int main(int, char[]) {
    std::ios_base::sync_with_stdio(false);

    config::Config config;

    std::cout << "Start HTTP Server on port " << config.port << "\n";

    http::Router router;
    router.add_route(http::Method::GET, "/", hello_wolrd)
          .add_route(http::Method::GET, "/ip", handle_ip);

    http::Server server(std::move(config), std::move(router));
    server.start();

    return 0;
}
