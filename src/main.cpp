#include <iostream>

#include "http/server.hpp"

using tcp = boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);

    config::Config config;

    std::cout << "Start HTTP Server on port " << config.port << "\n";

    http::Server server(std::move(config));
    server.start();

    return 0;
}
