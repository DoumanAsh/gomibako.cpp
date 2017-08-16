#include <iostream>
#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "server.hpp"
#include "../utils/format.hpp"

namespace http {
    using namespace boost::beast::http;
}

using namespace http;
using tcp = boost::asio::ip::tcp;

class HttpListener {
    private:
        tcp::acceptor& acceptor;
        tcp::socket socket{acceptor.get_io_service()};

        //Stores read data
        boost::beast::flat_static_buffer<8192> int_buffer;

        boost::beast::http::request<boost::beast::http::dynamic_body> request;
        boost::beast::http::response<boost::beast::http::dynamic_body> response;

    public:
        HttpListener(tcp::acceptor& acceptor) :
            acceptor(acceptor),
            socket(acceptor.get_io_service())
        {
            this->accept();
        }

        /**
         * Handle new incoming connection.
         */
        void accept() {
            this->acceptor.async_accept(this->socket, [this](boost::system::error_code error) {
                if (error) {
                    utils::log_error(error, "Cannot accept incoming TCP");
                }
                else {
                    this->handle_request();
                }

                //We continue accepting further requests to not leave
                //us without work.
                this->accept();
            });
        }

        /**
         * Handles HTTP Request.
         */
        void handle_request() {
            this->response.set(http::field::content_type, "text/html");
            boost::beast::ostream(this->response.body) << "<h1>Hello world</h1>\n";

            this->response.set(http::field::content_length, this->response.body.size());
            http::async_write(this->socket, this->response,
            [this](boost::system::error_code error)
            {
                this->socket.shutdown(tcp::socket::shutdown_send, error);
            });

        }
};

Server::Server(config::Config&& config) noexcept : config(std::move(config)) {}

void Server::start() {
    boost::asio::io_service ios(this->config.cpu_num);
    tcp::acceptor acceptor{ios, {this->config.address, this->config.port}};

    HttpListener listener(acceptor);

    ios.run();
}
