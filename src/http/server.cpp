#include <iostream>
#include <sstream>
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

        ///5kb for reading incoming data
        ///Allocated on stack
        using static_buffer_type = boost::beast::flat_static_buffer<5120>;

        //Stores read data
        static_buffer_type read_buffer;

        //As we need to keep request/response alive as long  as async ops are in place
        //we store them  inside of listener which is supposed to be alive
        //We can use optional to quickly clear stuff using optional->emplace()
        std::optional<http::request<http::dynamic_body>> request;
        //TODO: Think about serialization http://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/using_http/serializer_stream_operations.html
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
                    this->read_request();
                }
            });
        }

        /**
         * Sends response back to client.
         *
         * Closes socket and accepts another connection.
         */
        void send_response() {
            http::async_write(this->socket, *this->response,
            [this](boost::system::error_code error) {
                this->socket.shutdown(tcp::socket::shutdown_send, error);
                this->socket.close(error);
                this->accept();
            });
        }

        /**
         * Sends response with error to client.
         *
         * @param[in] error Error type. It is inserted into response using stream operator.
         * @param[in] status HTTP Status code. Default is 500.
         */
        template <typename Error>
        void send_error_response(Error&& error, http::status status = http::status::internal_server_error) {
            this->response.emplace();
            this->response->result(status);
            this->response->set(http::field::content_type, "text/plain");
            boost::beast::ostream(this->response->body) << error;
            this->response->prepare_payload();

            this->send_response();
        }

        void read_request() {
            this->request.emplace();

            http::async_read(this->socket, this->read_buffer, *this->request,
            [this](boost::system::error_code error) {
                if (error) {
                    this->send_error_response(std::move(error));
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
            if (this->request->method() != http::verb::get) {
                return this->send_error_response("Bad method", http::status::bad_request);
            }

            this->response.emplace();

            this->response->result(http::status::ok);
            this->response->set(http::field::content_type, "text/html");
            boost::beast::ostream(this->response->body) << "<h1>Hello world</h1>\n";
            this->response->prepare_payload();

            this->send_response();
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
