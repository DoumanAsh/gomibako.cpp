#include <boost/test/unit_test.hpp>

#include "../../src/utils/convert.hpp"

BOOST_AUTO_TEST_CASE(should_convert_number) {
    const auto unsigned_result = utils::convert<unsigned>("123");

    BOOST_REQUIRE(unsigned_result.has_value());
    BOOST_REQUIRE_EQUAL(*unsigned_result, (unsigned)123);
}

BOOST_AUTO_TEST_CASE(should_not_convert_number) {
    auto result = utils::convert<unsigned>("1-23");
    BOOST_REQUIRE_EQUAL(result.has_value(), false);

    result = utils::convert<unsigned>("12x3");
    BOOST_REQUIRE_EQUAL(result.has_value(), false);

    result = utils::convert<unsigned>("g123");
    BOOST_REQUIRE_EQUAL(result.has_value(), false);

    result = utils::convert<unsigned>("123a");
    BOOST_REQUIRE_EQUAL(result.has_value(), false);
}

BOOST_AUTO_TEST_CASE(should_handle_overflow_number) {
    const auto result = utils::convert<unsigned>("5555423555");

    BOOST_REQUIRE_EQUAL(result.has_value(), false);
}
