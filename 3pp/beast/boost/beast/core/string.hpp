//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_STRING_HPP
#define BOOST_BEAST_STRING_HPP

#include <boost/beast/config.hpp>
#include <boost/version.hpp>
#ifndef BOOST_BEAST_NO_BOOST_STRING_VIEW
# if BOOST_VERSION >= 106400
#  define BOOST_BEAST_NO_BOOST_STRING_VIEW 0
# else
#  define BOOST_BEAST_NO_BOOST_STRING_VIEW 1
# endif
#endif

#if BOOST_BEAST_NO_BOOST_STRING_VIEW
#include <boost/utility/string_ref.hpp>
#else
#include <boost/utility/string_view.hpp>
#endif

#include <algorithm>

namespace boost {
namespace beast {

#if BOOST_BEAST_NO_BOOST_STRING_VIEW
/// The type of string view used by the library
using string_view = boost::string_ref;

/// The type of basic string view used by the library
template<class CharT, class Traits>
using basic_string_view =
    boost::basic_string_ref<CharT, Traits>;
#else
/// The type of string view used by the library
using string_view = boost::string_view;

/// The type of basic string view used by the library
template<class CharT, class Traits>
using basic_string_view =
    boost::basic_string_view<CharT, Traits>;
#endif

namespace detail {

inline
char
ascii_tolower(char c)
{
    if(c >= 'A' && c <= 'Z')
        c += 'a' - 'A';
    return c;
}

template<class = void>
bool
iequals(
    beast::string_view lhs,
    beast::string_view rhs)
{
    auto n = lhs.size();
    if(rhs.size() != n)
        return false;
    auto p1 = lhs.data();
    auto p2 = rhs.data();
    char a, b;
    while(n--)
    {
        a = *p1++;
        b = *p2++;
        if(a != b)
            goto slow;
    }
    return true;

    while(n--)
    {
    slow:
        if(ascii_tolower(a) != ascii_tolower(b))
            return false;
        a = *p1++;
        b = *p2++;
    }
    return true;
}

} // detail

/** Returns `true` if two strings are equal, using a case-insensitive comparison.

    The case-comparison operation is defined only for low-ASCII characters.

    @param lhs The string on the left side of the equality

    @param rhs The string on the right side of the equality
*/
inline
bool
iequals(
    beast::string_view lhs,
    beast::string_view rhs)
{
    return detail::iequals(lhs, rhs);
}

/** A case-insensitive less predicate for strings.

    The case-comparison operation is defined only for low-ASCII characters.
*/
struct iless
{
    bool
    operator()(
        string_view lhs,
        string_view rhs) const
    {
        using std::begin;
        using std::end;
        return std::lexicographical_compare(
            begin(lhs), end(lhs), begin(rhs), end(rhs),
            [](char c1, char c2)
            {
                return detail::ascii_tolower(c1) < detail::ascii_tolower(c2);
            }
        );
    }
};

/** A case-insensitive equality predicate for strings.

    The case-comparison operation is defined only for low-ASCII characters.
*/
struct iequal
{
    bool
    operator()(
        string_view lhs,
        string_view rhs) const
    {
        return iequals(lhs, rhs);
    }
};

} // beast
} // boost

#endif
