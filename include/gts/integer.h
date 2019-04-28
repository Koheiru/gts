#pragma once
/*
#include <fas/type_list.hpp>
#include "common.h"
#include "error.h"

// ----------------------------------------------------------------------------
namespace gts {

// ----------------------------------------------------------------------------
template <class T>
class serializer<value<T>, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, void>::type>
{
public:
    template <class Iterator>
    static Iterator serialize(T value, Iterator pos)
    {
        using CharT = typename ci_traits<Iterator>::char_type;

        if (value == 0) {
            *(pos++) = CharT{ '0' };
            return pos;
        }

        if (std::is_signed<T>::value && value < 0)
            *(pos++) = CharT{ '-' };

        std::array<CharT, std::numeric_limits<T>::digits10 + 1> buffer;
        auto it = buffer.begin();
        for (; value != 0; value /= 10, ++it) {
            const char v = char(value % 10);
            *it = CharT{ '0' + (v > 0 ? v : -v) };
        }
        for (auto p = std::make_reverse_iterator(it); p != buffer.rend(); ++p) {
            *(pos++) = *p;
        }
        return pos;
    }

    template <class Iterator, class V = typename std::enable_if<std::is_unsigned<T>::value, void>::type>
    static Iterator unserialize(T& value, Iterator pos, Iterator end, parse_error* error)
    {
        return make_parse_error(pos, end, error_code::some_error, error);
    }

    template <class Iterator, class V = typename std::enable_if<std::is_signed<T>::value, void>::type>
    static Iterator unserialize(T& value, Iterator pos, Iterator end, parse_error* error)
    {
        using CharT = typename ci_traits<Iterator>::char_type;

        if (pos == end)
            return make_parse_error(pos, end, error_code::some_error, error);

        const bool is_negative = (*pos == CharT{ '-' });
        if (is_negative) {
            if (!std::is_signed<T>::value)
                return make_parse_error(pos, end, error_code::some_error, error);

            ++pos;
            if (pos == end)
                return make_parse_error(pos, end, error_code::some_error, error);
        }

        if (*pos < CharT{ '0' } || *pos > CharT{ '9' })
            return make_parse_error(pos, end, error_code::some_error, error);

        value = 0;
        for (size_t i = 0; i < std::numeric_limits<T>::digits10; ++i) {
            value = value * 10 + (*(pos++) - CharT{ '0' });
            if (pos == end || *pos < CharT{ '0' } || *pos > CharT{ '9' }) {
                if (is_negative)
                    value = -value;
                return pos;
            }
        }

        const T num = (*(pos++) - CharT{ '0' });
        const T limit = (std::is_signed<T>::value && is_negative) ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
        if (is_negative) {
            value = -value;
            if (limit / 10 > value || (limit / 10 == value && limit % 10 > num))
                return make_parse_error(pos, end, error_code::some_error, error);
            value = value * 10 - num;
        }
        else {
            if (limit / 10 < value || (limit / 10 == value && limit % 10 < num))
                return make_parse_error(pos, end, error_code::some_error, error);
            value = value * 10 + num;
        }

        if (pos == end || *pos < CharT{ '0' } || *pos > CharT{ '9' }) {
            return pos;
        }

        return make_parse_error(pos, end, error_code::some_error, error);
    }

private:

    T apply_minus(T value)
    {
    }
};

} // namespace gts
*/