#pragma once

#include <optional>

#include <boost/lexical_cast.hpp>

namespace utils {
    /**
     * Converts between types using boost::lexical_cast
     *
     * @param[in] arg Source value
     *
     * @returns Converted value.
     * @retval std::nullopt On failure
     */
    template <typename Target, typename Source>
    inline std::optional<Target> convert(const Source &arg)
    {
        Target result;

        return boost::conversion::try_lexical_convert(arg, result) ? std::make_optional(result) : std::nullopt;
    }
}
