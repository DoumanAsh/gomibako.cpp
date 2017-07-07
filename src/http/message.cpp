#include <inttypes.h>
#include <stdexcept>

#include "message.hpp"
using namespace http;

const char* method_to(Method method) {
    switch (method) {
        case Method::GET: return "GET";
        case Method::HEAD: return "HEAD";
        case Method::POST: return "POST";
        case Method::PUT: return "PUT";
        case Method::DEL: return "DELETE";
        case Method::CONNECT: return "CONNECT";
        case Method::OPTIONS: return "OPTIONS";
        case Method::TRACE: return "TRACE";
    }

    return "UNKNOWN";
}

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

Method Request::method() const {
#define IS_METHOD(_method) (mg_vcmp(&this->inner->method, _method) == 0)
    if (IS_METHOD("GET")) return Method::GET;
    else if (IS_METHOD("HEAD")) return Method::HEAD;
    else if (IS_METHOD("POST")) return Method::POST;
    else if (IS_METHOD("PUT")) return Method::PUT;
    else if (IS_METHOD("DELETE")) return Method::DEL;
    else if (IS_METHOD("CONNECT")) return Method::CONNECT;
    else if (IS_METHOD("OPTIONS")) return Method::OPTIONS;
    else if (IS_METHOD("TRACE")) return Method::TRACE;

    //unreachable normally
    throw std::runtime_error("Unknown method!");
#undef IS_METHOD
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
