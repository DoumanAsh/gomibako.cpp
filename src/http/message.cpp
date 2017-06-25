#include <inttypes.h>

#include "message.hpp"
using namespace http;

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
