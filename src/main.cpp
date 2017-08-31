#include <iostream>

#include "http/server.hpp"

using tcp = boost::asio::ip::tcp;

void hello_wolrd(http::Router::Context&& ctx) {
    ctx.response.result(http::status::ok);
    ctx.response.set(http::field::content_type, "text/html");
    boost::beast::ostream(ctx.response.body) << "<h1>Hello world</h1>\n";
}

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);

    config::Config config;

    std::cout << "Start HTTP Server on port " << config.port << "\n";

    http::Router router;
    router.add_route(http::Method::GET, "/", hello_wolrd);

    http::Server server(std::move(config), std::move(router));
    server.start();

    return 0;
}
