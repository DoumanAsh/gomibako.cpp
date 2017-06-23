#include <stdexcept>

#include "http.hpp"

using namespace http;

static inline void handle_http_request(struct mg_connection *connection, void *message) {
    const auto self = static_cast<Server*>(connection->mgr->user_data);
    const auto http_msg = static_cast<struct http_message*>(message);

    const auto routes = self->get_routes();

    for (auto &route : routes) {
        if (mg_vcmp(&http_msg->uri, route.first) == 0) {
            route.second(connection, http_msg);
            return;
        }
    }

    const auto def_route = self->get_default_route();

    if (def_route != nullptr) {
        def_route(connection, http_msg);
    }
}

static void ev_handler(struct mg_connection *connection, int event, void *message) {
    switch (event) {
        case MG_EV_HTTP_REQUEST:
            handle_http_request(connection, message);
            break;
        default:
            break;
    }
}

static const char *default_addr = "80";

Server::Server() : Server(default_addr) {}

Server::Server(const char* addr) : max_sleep(1000), http_options({0}) {
    mg_mgr_init(&this->manager, this);
    this->conn = mg_bind(&this->manager, addr, ev_handler);

    if (this->conn == NULL) {
        throw std::runtime_error("Failed to create listener");
    }

    mg_set_protocol_http_websocket(this->conn);
    this->http_options.enable_directory_listing = "no";
}

Server::~Server() {
    mg_mgr_free(&this->manager);
}

void Server::start() {
    for (;;) mg_mgr_poll(&this->manager, 1000);
}

void Server::set_sleep_time(int ms) {
    this->max_sleep = ms;
}

void Server::set_serve_dir(const char *dir) {
    this->set_serve_dir(dir, true);
}

void Server::set_serve_dir(const char *dir, bool is_listing) {
    this->http_options.document_root = dir;
    this->http_options.enable_directory_listing = is_listing ? "yes" : "no";
}

const struct mg_serve_http_opts* Server::get_http_options() const {
    return &this->http_options;
}

void Server::add_route(const char* path, http_route_hander handler) {
    this->routes.emplace_back(path, handler);
}

const std::vector<http_route>& Server::get_routes() const {
    return this->routes;
}

void Server::add_default_route(http_route_hander handler) {
    this->default_route = handler;
}

const http_route_hander Server::get_default_route() const {
    return this->default_route;
}
