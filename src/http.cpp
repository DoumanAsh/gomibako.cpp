#include <stdexcept>

#include "http.hpp"

using namespace http;

static struct mg_serve_http_opts http_options;

static void ev_handler(struct mg_connection *connection, int event, void *message) {
    switch (event) {
        case MG_EV_HTTP_REQUEST:
            mg_serve_http(connection, (struct http_message*)message, http_options);
            break;
        default:
            break;
    }
}


static const char *default_addr = "80";

Server::Server() : Server(default_addr) {}

Server::Server(const char* addr) : max_sleep(1000) {
    mg_mgr_init(&this->manager, NULL);
    this->conn = mg_bind(&this->manager, addr, ev_handler);

    if (this->conn == NULL) {
        throw std::runtime_error("Failed to create listener");
    }

    mg_set_protocol_http_websocket(this->conn);
}

Server::~Server() {
    mg_mgr_free(&this->manager);
}

void Server::start() {
    for (;;) {
        mg_mgr_poll(&this->manager, 1000);
    }
}

void Server::set_sleep_time(int ms) {
    this->max_sleep = ms;
}

void Server::set_serve_dir(const char *dir) {
    this->set_serve_dir(dir, true);
}

void Server::set_serve_dir(const char *dir, bool is_listing) {
    http_options.document_root = dir;
    http_options.enable_directory_listing = is_listing ? "yes" : "no";
}
