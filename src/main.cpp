#include <iostream>

#include "http.hpp"

static void handle_404(struct mg_connection *connection, struct http_message* message) {
    (void)message;

    const char msg[] = "<h1>Not found</h1>";
    mg_send_head(connection, 404, sizeof(msg) - 1, "Content-Type: text/html");
    mg_printf(connection, "%s", msg);
    //TODO: write wrapper function.
    connection->flags |= MG_F_SEND_AND_CLOSE;
}

int main(int argc, char *argv[]) {
    http::Server server;

    server.add_default_route(handle_404);
    server.start();

    return 0;
}
