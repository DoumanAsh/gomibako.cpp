#include <iostream>
#include <random>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "http/server.hpp"
#include "http/typed_headers.hpp"

#include "utils/convert.hpp"

using tcp = boost::asio::ip::tcp;

/**
 * Root page
 */
void hello_world(http::Router::Context&& ctx) {
    ctx.response.result(http::status::ok);
    http::header::set(ctx.response, http::header::ContentType::html());
    boost::beast::ostream(ctx.response.body()) << "<h1>Hello world</h1>\n";
}

/**
 * Handler for `/ip` route
 */
void handle_ip(http::Router::Context&& ctx) {
    boost::property_tree::ptree content;
    content.put("ip", ctx.socket.remote_endpoint().address().to_string());

    ctx.response.result(http::status::ok);
    http::header::set(ctx.response, http::header::ContentType::json());
    auto body_stream = boost::beast::ostream(ctx.response.body());
    boost::property_tree::write_json(body_stream, content, false);
}

/**
 * Handler for `/headers` route
 */
void handle_headers(http::Router::Context&& ctx) {
    ctx.response.result(http::status::ok);

    boost::property_tree::ptree content;
    http::header::set(ctx.response, http::header::ContentType::json());
    for (const auto& header : ctx.request) {
        content.put(std::string(header.name_string()), header.value());
    }
    auto body_stream = boost::beast::ostream(ctx.response.body());
    boost::property_tree::write_json(body_stream, content, false);
}

/**
 * Handler for `/status/:code` route
 */
void handle_status(http::Router::Context&& ctx) {
    const auto code = utils::convert<unsigned>(ctx.matches.at("code"));

    if (code) {
        try {
            ctx.response.result(*code);
        }
        catch (const std::invalid_argument& error) {
            ctx.response.result(http::status::bad_request);
            boost::beast::ostream(ctx.response.body()) << error.what()
                                                       << "\n";
        }
    }
    else {
        ctx.response.result(http::status::bad_request);
        boost::beast::ostream(ctx.response.body()) << "status-code is not a number.\n";
    }
}

/**
 * Handler for `/bytes/:num`.
 *
 * Limited by http::STATIC_BUFFER_MAX.
 */
void handle_bytes(http::Router::Context&& ctx) {
    const auto num = utils::convert<size_t>(ctx.matches.at("num"));

    if (num) {
        //TODO: Use chunked response here?
        if (*num > http::STATIC_BUFFER_MAX) {
            ctx.response.result(http::status::bad_request);
            boost::beast::ostream(ctx.response.body()) << "Bytes limit is "
                                                       << http::STATIC_BUFFER_MAX
                                                       << "\n";
        }
        else {
            ctx.response.result(http::status::ok);
            http::header::set(ctx.response, http::header::ContentType::octet_stream());

            auto out = boost::beast::ostream(ctx.response.body());
            std::random_device rd;

            for (size_t idx = *num; idx != 0u; idx--)
            {
                out << static_cast<unsigned char>(rd());
            }
        }
    }
    else {
        ctx.response.result(http::status::bad_request);
        boost::beast::ostream(ctx.response.body()) << "Bytes is not a number.\n";
    }
}

int main(int /*argc*/, char* /*argv[]*/) {
    std::ios_base::sync_with_stdio(false);

    config::Config config;

    std::cout << "Start HTTP Server on port " << config.port << "\n";

    http::Router router;
    router.add_route(http::Method::GET, "/", hello_world)
          .add_route(http::Method::GET, "/ip", handle_ip)
          .add_route(http::Method::GET, "/headers", handle_headers)
          .add_route(http::Method::GET, "/status/:code", handle_status)
          .add_route(http::Method::GET, "/bytes/:num", handle_bytes);

    http::Server server(std::move(config), std::move(router));
    server.start();

    return 0;
}
