#include <string>
#include <sstream>

#include "http.hpp"

static void handle_404(http::Response& response, const http::Request& req) {
    (void)req;

    const char body[] = "<h1>Not found</h1>";
    response.status_code = 404;
    response.len = sizeof(body) - 1;

    std::stringstream input;
    input << "Content-Type: text/html" << http::header_end
          << http::header_end
          << body;
    input >> response.start();
}

static void route_ip(http::Response& response, const http::Request& req) {
    response.status_code = 200;
    //TODO: investigate crash in mongoose when attempting to get remote IP.
    const auto remote_ip = req.remote_ip();

    std::cout << remote_ip << std::endl;
}

int main(int argc, char *argv[]) {
    http::Server server;

    server.add_default_route(handle_404);
    server.add_route("/ip", route_ip);
    server.start();

    return 0;
}
