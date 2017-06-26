#include <inttypes.h>

#include "message.hpp"
using namespace http;

Request::Request(struct http_message *msg, struct mg_connection*) : inner(msg), conn(conn) {}

const char* Request::body() const {
    return this->inner->body.p;
}

const char* Request::method() const {
    return this->inner->method.p;
}

const char* Request::uri() const {
    return this->inner->uri.p;
}

const char* Request::query_string() const {
    return this->inner->query_string.p;
}

std::string Request::remote_ip() const {
    char buffer[46];

    mg_conn_addr_to_str(this->conn, buffer, sizeof(buffer), MG_SOCK_STRINGIFY_IP);

    return std::string(buffer);
}

std::istream& http::operator>>(std::istream &stream, Response &self) {
    char buffer[1024];

    while (stream.read(buffer, sizeof(buffer))) mg_send(self.conn, buffer, static_cast<int>(sizeof(buffer)));
    //write remaining stuff
    mg_send(self.conn, buffer, static_cast<int>(stream.gcount()));

    return stream;
}

Response::Response(struct mg_connection *conn) : Response(conn, 500) {}

Response::Response(struct mg_connection *conn, int status) : Response(conn, status, 0) {}

Response::Response(struct mg_connection *conn, int status, int64_t len) : conn(conn), status_code(status), len(len) {}

Response& Response::start() {
    mg_printf(this->conn,
              "HTTP/1.1 %d\r\n"
              "Content-Length: %" PRIu64 "\r\n",
              this->status_code, this->len);
    return *this;
}

Response::~Response() {
    this->conn->flags |= MG_F_SEND_AND_CLOSE;
}
