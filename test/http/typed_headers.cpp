#include <string>
#include <iostream>

#include "../../src/http/typed_headers.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(should_correctly_create_weak_etag)
{
    std::string tag = "OLOLO";
    bool weak = true;
    http::ETag etag(tag, weak);

    BOOST_REQUIRE_EQUAL(etag.tag, tag);
    BOOST_REQUIRE_EQUAL(etag.weak, weak);
    BOOST_REQUIRE_EQUAL(etag.name(), boost::beast::http::field::etag);

    boost::beast::string_param expected_value("W/\"", tag, "\"");
    //TODO: find out why etag.value() produces bad str
    std::cout << "value='" << etag.value().str() << "' | "
              << "expected_value='" << expected_value.str() << "'\n";
    BOOST_REQUIRE_EQUAL(expected_value.str(), etag.value().str());
}
