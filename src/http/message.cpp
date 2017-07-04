#include <inttypes.h>

#include "message.hpp"
using namespace http;

//Headers
Headers::Headers(struct mg_str* names, struct mg_str* values) : idx(0), names(names), values(values) {}
Headers::Headers(const Headers& it) : idx(it.idx), names(it.names), values(it.values) {}

std::optional<header> Headers::get(const char* name) {
    size_t name_len = strlen(name);

    for (size_t i = 0; this->names[i].len > 0; i++) {
        if (this->names[i].len == name_len && !mg_ncasecmp(this->names[i].p, name, name_len)) {
            return std::make_pair(std::string(this->names[i].p, this->names[i].len), std::string(this->values[i].p, this->values[i].len));
        }
    }

    return std::nullopt;
}

std::optional<http::header> Headers::next() {
    auto name = this->names[this->idx], value = this->values[this->idx];

    if (name.len > 0 && name.p != NULL) {
        this->idx += 1;

        return std::make_pair(std::string(name.p, name.len), std::string(value.p, value.len));
    }
    else {
        return std::nullopt;
    }
}

std::optional<http::header> Headers::prev() {
    auto name = this->names[this->idx], value = this->values[this->idx];

    if (name.len > 0 && name.p != NULL) {
        if (this->idx > 0) this->idx--;

        return std::make_pair(std::string(name.p, name.len), std::string(value.p, value.len));
    }
    else {
        return std::nullopt;
    }
}

//Request
Request::Request(struct http_message *msg, struct mg_connection *conn) : inner(msg), conn(conn) {}

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

const char* Request::header(const char* name) const {
    const auto header = mg_get_http_header(this->inner, name);

    return header == NULL ? nullptr : header->p;
}

std::string Request::remote_ip() const {
    char buffer[46];

    mg_conn_addr_to_str(this->conn, buffer, sizeof(buffer), MG_SOCK_STRINGIFY_IP);

    return std::string(buffer);
}

Headers Request::headers() const {
    return Headers(this->inner->header_names, this->inner->header_values);
}

//Response
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
