#include <iostream>
#include <memory>
#include <optional>

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

        //As we need to keep request/response alive as long  as async ops are in place
        //we store them  inside of listener which is supposed to be alive
        //We can use optional to quickly clear stuff using optional->emplace()
        //TODO: Think about serialization http://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/using_http/serializer_stream_operations.html
        std::optional<http::request<http::dynamic_body>> request;
        std::optional<http::response<http::dynamic_body>> response;

    public:
        HttpListener(tcp::acceptor& acceptor) :
            acceptor(acceptor)
        {
            this->accept();
        }

        /**
         * Handle new incoming connection.
         *
         * After finishing handling each new connection the function is called once again.
         * The function should be called only after request's handling is finished!
         * As long as it is called there will be jobs to do for boost::io_service and it will loop through them forever.
         */
        void accept() {
            this->acceptor.async_accept(this->socket, [this](boost::system::error_code error) {
                if (error) {
                    utils::log_error(error, "Cannot accept incoming TCP");
                    this->accept();
                }
                else {
                    this->handle_request();
                }
            });
        }

        /**
         * Handles HTTP Request.
         *
         * TODO: Next step is to think of API to provide routing.
         */
        void handle_request() {
            this->response.emplace();

            this->response->set(http::field::content_type, "text/html");
            boost::beast::ostream(this->response->body) << "<h1>Hello world</h1>\n";

            this->response->set(http::field::content_length, this->response->body.size());
            http::async_write(this->socket, *this->response,
            [this](boost::system::error_code error)
            {
                this->socket.shutdown(tcp::socket::shutdown_send, error);
                this->socket.close(error);
                this->accept();
            });

        }
};

Server::Server(config::Config&& config) noexcept : config(std::move(config)) {}

void Server::start() {
    boost::asio::io_service ios(this->config.cpu_num);
    tcp::acceptor acceptor{ios, {this->config.address, this->config.port}};

    //Consider running few more listeners?
    HttpListener listener(acceptor);

    ios.run();
}
