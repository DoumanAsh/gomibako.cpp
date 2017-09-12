#include <string>
#include <iostream>

#include "../../src/http/typed_headers.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(should_correctly_create_weak_etag) {
    std::string tag = "OLOLO";
    bool weak = true;
    http::header::ETag etag(tag, weak);

    BOOST_REQUIRE_EQUAL(etag.tag, tag);
    BOOST_REQUIRE_EQUAL(etag.weak, weak);
    BOOST_REQUIRE_EQUAL(etag.name(), boost::beast::http::to_string(boost::beast::http::field::etag));

    std::string expected_value = "W/\"" + tag + "\"";
    BOOST_REQUIRE_EQUAL(expected_value, etag.value());
}

BOOST_AUTO_TEST_CASE(should_correctly_create_strong_etag) {
    std::string tag = "OLOLO";
    bool weak = false;
    http::header::ETag etag(tag, weak);

    BOOST_REQUIRE_EQUAL(etag.tag, tag);
    BOOST_REQUIRE_EQUAL(etag.weak, weak);
    BOOST_REQUIRE_EQUAL(etag.name(), boost::beast::http::to_string(boost::beast::http::field::etag));

    std::string expected_value = "\"" + tag + "\"";
    BOOST_REQUIRE_EQUAL(expected_value, etag.value());
}

BOOST_AUTO_TEST_CASE(should_parse_weak_etag) {
    std::string etag_str = "W/\"loli\"";
    auto etag = http::header::ETag::parse(etag_str);

    BOOST_REQUIRE(etag.has_value());
    BOOST_REQUIRE_EQUAL(etag->tag, "loli");
    BOOST_REQUIRE_EQUAL(etag->weak, true);
    BOOST_REQUIRE_EQUAL(etag->name(), boost::beast::http::to_string(boost::beast::http::field::etag));

    BOOST_REQUIRE_EQUAL(etag_str, etag->value());
}

BOOST_AUTO_TEST_CASE(should_parse_strong_etag) {
    std::string etag_str = "\"loli\"";
    auto etag = http::header::ETag::parse(etag_str);

    BOOST_REQUIRE(etag.has_value());
    BOOST_REQUIRE_EQUAL(etag->tag, "loli");
    BOOST_REQUIRE_EQUAL(etag->weak, false);
    BOOST_REQUIRE_EQUAL(etag->name(), boost::beast::http::to_string(boost::beast::http::field::etag));

    BOOST_REQUIRE_EQUAL(etag_str, etag->value());
}
