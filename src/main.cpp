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
    const auto remote_ip = req.remote_ip();

    response.status_code = 200;
    response.len = remote_ip.size();

    std::stringstream input;
    input << "Content-Type: text/plain" << http::header_end
          <<http::header_end
          << remote_ip;

    input >> response.start();
}

int main(int argc, char *argv[]) {
    http::Server server;

    server.add_default_route(handle_404);
    server.add_route("/ip", route_ip);
    server.start();

    return 0;
}
