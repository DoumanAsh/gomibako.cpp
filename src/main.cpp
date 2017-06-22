#include <iostream>

#include "http.hpp"

int main(int argc, char *argv[]) {
    http::Server server;

    server.set_serve_dir(".");
    server.start();

    return 0;
}
