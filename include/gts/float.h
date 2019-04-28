#pragma once

#include <double-conversion/double-conversion.h>
#include "common.h"
#include "error.h"

// ----------------------------------------------------------------------------
//! TODO: implement Grisu3 algorithm.

// ----------------------------------------------------------------------------
namespace gts {

// ----------------------------------------------------------------------------
template <class T>
class serializer<value<T>, typename std::enable_if<std::is_floating_point<T>::value, void>::type>
{
public:
    template <class Iterator>
    static Iterator serialize(T value, Iterator pos)
    {
        using CharT = typename ci_traits<Iterator>::char_type;
        
        using namespace double_conversion;
        constexpr int flags = DoubleToStringConverter::NO_FLAGS;
        constexpr const char* const inf = "Inf";
        constexpr const char* const nan = "NaN";
        constexpr char exp = 'e';
        constexpr int decimal_in_shortest_low  = -6;
        constexpr int decimal_in_shortest_high = 21;

        constexpr size_t bufferSize = 128;
        char buffer[bufferSize] = { 0 };
        StringBuilder builder(buffer, bufferSize);

        DoubleToStringConverter cvt(flags, inf, nan, exp, decimal_in_shortest_low, decimal_in_shortest_high, 0, 0);
        cvt.ToShortest(value, &builder);

        const char* ch = builder.Finalize();
        while (*ch != '\0')
            *(pos++) = CharT{ *(ch++) };

        return pos;
    }

    template <class Iterator>
    static Iterator unserialize(T& value, Iterator pos, Iterator end, parse_error* error)
    {
        using CharT = typename ci_traits<Iterator>::char_type;

        if (pos == end)
            return make_parse_error(pos, end, error_code::some_error, error);

        using namespace double_conversion;
        constexpr int flags = StringToDoubleConverter::ALLOW_CASE_INSENSIBILITY;
        constexpr double empty_string_value = 0.0;
        constexpr double junk_string_value = 0.0;
        constexpr const char* const inf = "Inf";
        constexpr const char* const nan = "NaN";

        constexpr size_t maxSignificantDigits = 772;
        constexpr size_t bufferSize = maxSignificantDigits + 10;
        char buffer[bufferSize] = { 0 };

        char* dst = buffer;
        while (pos != end) {
            *(dst++) = *(pos++);
        }
        *dst = '\0';


        StringToDoubleConverter cvt(flags, empty_string_value, junk_string_value, inf, nan);
        int processed_characters_count{};
        value = cvt.StringToDouble((char*)buffer, std::distance((char*)buffer, dst), &processed_characters_count);
        return pos;
    }
};

} // namespace gts
