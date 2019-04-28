#pragma once
/*
#include <fas/type_list.hpp>
#include "common.h"
#include "error.h"

// ----------------------------------------------------------------------------
namespace gts {

// ----------------------------------------------------------------------------
template <>
class serializer<value<bool>>
{
public:
    template <class Iterator>
    static Iterator serialize(bool value, Iterator pos)
    {
        using CharT = typename ci_traits<Iterator>::char_type;
        constexpr string_literal<CharT, 6> true_str("true");
        constexpr string_literal<CharT, 6> false_str("false");

        const auto& str = (value ? true_str : false_str);
        for (size_t i = 0; i < str.size(); ++i)
            *(pos++) = str[i];

        return pos;
    }

    template <class Iterator>
    static Iterator unserialize(bool& value, Iterator pos, Iterator end, parse_error* error)
    {
        using CharT = typename ci_traits<Iterator>::char_type;
        constexpr string_literal<CharT, 6> true_str("true");
        constexpr string_literal<CharT, 6> false_str("false");

        if (pos == end)
            return make_parse_error(pos, end, error_code::some_error, error);

        const CharT ch = *(pos++);
        if (ch != true_str[0] && ch != false_str[0])
            return make_parse_error(pos, end, error_code::some_error, error);

        const auto& str = (ch == true_str[0]) ? (true_str) : (false_str);
        for (size_t i = 1; i < str.size(); ++i) {
            if (pos == end)
                return make_parse_error(pos, end, error_code::some_error, error);
            if (*(pos++) != str[i])
                return make_parse_error(pos, end, error_code::some_error, error);
        }

        value = (ch == true_str[0]);
        return pos;
    }
};

} // namespace gts
*/