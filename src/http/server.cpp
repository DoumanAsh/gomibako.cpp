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

/**
 * HTTP boost::io_service handler.
 *
 * Object of this class should live as long as boost::io_service::run() is executing.
 * It can be achieved through different means:
 * * Use shared_ptr to control lifetime and refer to itself from each async closure.
 * * Just create object on stack and let it live long enough (Maybe a bad idea to put it in heap in multithreaded environment).
 *
 * First can case can be found in multiple examples.
 * But the second works as charm even in multithreaded environment.
 * So why settle for unnecessary overhead of shared_ptr?
 *
 * TODO: There is a question whether you'd need a synchronization for pure heap-allocated members.
 */
class HttpListener {
    private:
        /**
         * Let us to accept incoming TCP connections.
         *
         * It can be shared across multiple instances of @ref HttpListener.
         * Acceptor should be already aware of our IP and port.
         */
        tcp::acceptor& acceptor;

        /**
         * Creates socket from reference to boost::io_service
         *
         * Socket is not yet opened at this moment.
         * Only after performing first accept it will be actually opened.
         */
        tcp::socket socket{acceptor.get_io_service()};

        ///4kb for reading incoming data
        ///Allocated on stack
        using static_buffer_type = boost::beast::flat_static_buffer<4096>;

        //Reading buffer for our async_read
        static_buffer_type read_buffer;

        //As we need to keep request/response alive as long  as async ops are in place
        //we store them  inside of listener which is supposed to be alive
        //We can use optional to quickly clear stuff using optional->emplace()
        std::optional<http::request<http::dynamic_body>> request;
        //TODO: Think about serialization http://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/using_http/serializer_stream_operations.html
        std::optional<http::response<http::dynamic_body>> response;

    public:
        //We don't really have any use without acceptor.
        HttpListener() = delete;

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
         * Finalize response by setting default headers.
         */
        inline void prepare_response() {
            this->response->set(http::field::server, "Gomibako");
            this->response->prepare_payload();
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

            this->prepare_response();
            this->send_response();
        }

        /**
         * Reads Client's request
         *
         * On failure to read it would make sense to send back some meaningful error response.
         */
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

            this->prepare_response();
            this->send_response();
        }
};

Server::Server(config::Config&& config) noexcept : config(std::move(config)) {}

void Server::start() {
    //Creates io_service and hints number of threads that can be used.
    //As it is thread safe you don't need to worry about running the same service in multiple threads.
    //Each thread that invokes io_service::run() will participate in distribution of incoming request.
    //Note that you can use io_service::strand to ensure that handlers are not going to run concurrently.
    boost::asio::io_service ios(this->config.cpu_num);
    //We create acceptor with tcp::endpoint by providing address and port.
    //By default reuse of address it true, so we can have multiple acceptors.
    tcp::acceptor acceptor{ios, {this->config.address, this->config.port}};

    //Consider running few more listeners?
    HttpListener listener(acceptor);

    ios.run();
}
