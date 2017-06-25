#include <string>
#include <sstream>

#include "http.hpp"

static void handle_404(http::Response& msg, struct http_message* message) {
    (void)message;

    const char body[] = "<h1>Not found</h1>";
    msg.status_code = 404;
    msg.len = sizeof(body) - 1;

    std::stringstream input;
    input << "Content-Type: text/html" << http::header_end
          << http::header_end
          << body;
    input >> msg.start();

    msg.end();
}

int main(int argc, char *argv[]) {
    http::Server server;

    server.add_default_route(handle_404);
    server.start();

    return 0;
}
