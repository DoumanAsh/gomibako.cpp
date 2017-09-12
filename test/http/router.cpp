#include "../../src/http/router.hpp"
#include <boost/test/unit_test.hpp>

void dummy_callback(http::Router::Context&&) {
}

BOOST_AUTO_TEST_CASE(should_match_root_route) {
    const http::Router::Route route("/", dummy_callback);

    auto result = route.match("/");
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE_EQUAL(result->size(), 0);

    result = route.match("/some");
    BOOST_REQUIRE_EQUAL(result.has_value(), false);
}

BOOST_AUTO_TEST_CASE(should_match_ip) {
    const http::Router::Route route("/ip", dummy_callback);

    auto result = route.match("/ip");
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE_EQUAL(result->size(), 0);

    result = route.match("/ip/");
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE_EQUAL(result->size(), 0);
}

BOOST_AUTO_TEST_CASE(should_capture_comp) {
    const http::Router::Route route("/:path", dummy_callback);
    auto result = route.match("/");
    BOOST_REQUIRE_EQUAL(result.has_value(), false);

    result = route.match("/some");
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE_EQUAL(result->size(), 1);
    BOOST_REQUIRE_EQUAL(result->at("path"), "some");

    result = route.match("/some/");
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE_EQUAL(result->size(), 1);
    BOOST_REQUIRE_EQUAL(result->at("path"), "some");
}

BOOST_AUTO_TEST_CASE(should_capture_multiple_comp) {
    const http::Router::Route route("/:path/second/:third", dummy_callback);

    auto result = route.match("/some/second/3");
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE_EQUAL(result->size(), 2);
    BOOST_REQUIRE_EQUAL(result->at("path"), "some");
    BOOST_REQUIRE_EQUAL(result->at("third"), "3");

    result = route.match("/some/second/3/");
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE_EQUAL(result->size(), 2);
    BOOST_REQUIRE_EQUAL(result->at("path"), "some");
    BOOST_REQUIRE_EQUAL(result->at("third"), "3");
}
