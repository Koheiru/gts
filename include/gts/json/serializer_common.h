#pragma once
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include "../details/static_string.h"
#include "../details/iterator_traits.h"
#include "../meta_type.h"

// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
namespace details
{

// ----------------------------------------------------------------------------
//! NOTE: these methods introduced to avoid compiler warning about signed/unsigned
//  comparison in the cases when T is unsigned type.
// ----------------------------------------------------------------------------
template <class T, class = void>
struct integrals
{
    static bool is_negative(const T& value) { return (value < 0); }
};

template <class T>
struct integrals<T, typename std::enable_if<std::is_unsigned<T>::value>::type>
{
    static bool is_negative(const T& value) { return false; }
};

template <class CharT, class Traits = std::char_traits<CharT>>
struct ostreambuf : public std::basic_streambuf<CharT, Traits>
{
    ostreambuf(CharT* buffer, std::streamsize bufferLength)
        : std::basic_streambuf<CharT, Traits>()
    {
        setp(buffer, buffer + bufferLength);
    }
};

// ----------------------------------------------------------------------------
template <class CharT, class Traits = std::char_traits<CharT>>
struct istreambuf : public std::basic_streambuf<CharT, Traits>
{
    istreambuf(CharT* buffer, std::streamsize bufferLength)
        : std::basic_streambuf<CharT, Traits>()
    {
        setg(buffer, buffer, buffer + bufferLength);
    }
};

// ----------------------------------------------------------------------------
class json_strokanizer
{
public:
    template <class Iterator, class StringIterator>
    static Iterator serialize(StringIterator cur, StringIterator end, Iterator dst)
    {
        using char_type = typename iterator_traits<Iterator>::value_type;
        using input_type = typename iterator_traits<StringIterator>::value_type;
        using uinput_type = typename std::make_signed<input_type>::type;

        *(dst++) = char_type{ '"' };
        for (; cur != end; ++cur) {
            const uinput_type ch = *cur;
            if (ch == 0x20 || ch == 0x21 || (ch >= 0x23 && ch <= 0x2E) || (ch >= 0x30 && ch <= 0x5B) || (ch >= 0x5D && ch <= 0xFF)) {
                *(dst++) = static_cast<char_type>(ch);
            }
            else if (ch == uinput_type{ '\b' }) *(dst++) = char_type{ '\\' }, *(dst++) = char_type{ 'b'  };
            else if (ch == uinput_type{ '\f' }) *(dst++) = char_type{ '\\' }, *(dst++) = char_type{ 'f'  };
            else if (ch == uinput_type{ '\n' }) *(dst++) = char_type{ '\\' }, *(dst++) = char_type{ 'n'  };
            else if (ch == uinput_type{ '\r' }) *(dst++) = char_type{ '\\' }, *(dst++) = char_type{ 'r'  };
            else if (ch == uinput_type{ '\t' }) *(dst++) = char_type{ '\\' }, *(dst++) = char_type{ 't'  };
            else if (ch == uinput_type{ '/'  }) *(dst++) = char_type{ '\\' }, *(dst++) = char_type{ '/'  };
            else if (ch == uinput_type{ '"'  }) *(dst++) = char_type{ '\\' }, *(dst++) = char_type{ '"'  };
            else if (ch == uinput_type{ '\\' }) *(dst++) = char_type{ '\\' }, *(dst++) = char_type{ '\\' };
            else {
                constexpr auto digits = make_static_string<char_type>("0123456789ABCDF");
                std::uint32_t u = std::min(static_cast<std::uint32_t>(ch), std::uint32_t{ 0xFFFF });
                unsigned long d1 = u / 4096; u -= d1 * 4096;
                unsigned long d2 = u / 256; u -= d2 * 256;
                unsigned long d3 = u / 16; u -= d3 * 16;
                unsigned long d4 = u;
                *(dst++) = char_type{ '\\' };
                *(dst++) = char_type{ 'u'  };
                *(dst++) = digits[d1];
                *(dst++) = digits[d2];
                *(dst++) = digits[d3];
                *(dst++) = digits[d4];
            }
        }
        *(dst++) = char_type{ '"' };
        return dst;
    }
};

} // namespace details

// ----------------------------------------------------------------------------
template <class T, class = void>
struct json_common;

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_common<value<bool, Args...>, void>
{
    using target = typename value<bool, Args...>::target;

    template <class IteratorT>
    static IteratorT serialize(target value, IteratorT it)
    {
        using char_type = typename iterator_traits<IteratorT>::value_type;
        constexpr auto true_value = make_static_string<char_type>("true");
        constexpr auto false_value = make_static_string<char_type>("false");
        return value ? std::copy(true_value.c_str(), true_value.c_str() + true_value.length(), it) 
            : std::copy(false_value.c_str(), false_value.c_str() + false_value.length(), it);
    }
};

// ----------------------------------------------------------------------------
template <class T, class ...Args>
struct json_common<value<T, Args...>, typename std::enable_if<std::is_integral<T>::value>::type>
{
    using target = typename value<T, Args...>::target;

    template <class IteratorT>
    static IteratorT serialize(target value, IteratorT it)
    {
        using char_type = typename iterator_traits<IteratorT>::value_type;
        constexpr auto digits = make_static_string<char_type>("0123456789");

        if (value == 0) {
            *(it++) = char_type{ '0' };
            return it;
        }

        if (details::integrals<T>::is_negative(value)) {
            value *= -1;
            *(it++) = char_type{ '-' };
        }

        std::array<char_type, std::numeric_limits<T>::digits10 + 1> buffer;
        auto pos = buffer.begin();
        for (; value != 0; value /= 10, ++pos) {
            *pos = digits[value % 10];
        }
        return std::copy(std::make_reverse_iterator(pos), buffer.rend(), it);
    }
};

// ----------------------------------------------------------------------------
template <class T, class ...Args>
struct json_common<value<T, Args...>, typename std::enable_if<std::is_floating_point<T>::value>::type>
{
    using target = typename value<T, Args...>::target;

    template <class IteratorT>
    static IteratorT serialize(const target& value, IteratorT it)
    {
        using char_type = typename iterator_traits<IteratorT>::value_type;

        //! TODO: rewrite this code...

        const std::string text = [&value]() {
            std::stringstream stream;
            stream << value;
            return stream.str();
        }();

        constexpr auto digits = make_static_string<char_type>("0123456789");
        for (const char digit : text) {
            if (digit == '.') {
                *(it++) = char_type{ '.' };
                continue;
            }
            if (digit == '+') {
                *(it++) = char_type{ '+' };
                continue;
            }
            if (digit == '-') {
                *(it++) = char_type{ '-' };
                continue;
            }
            if (digit == 'E') {
                *(it++) = char_type{ 'E' };
                continue;
            }
            if (digit == 'e') {
                *(it++) = char_type{ 'e' };
                continue;
            }
            *(it++) = digits[digit - '0'];
        }

        return it;
    }
};

// ----------------------------------------------------------------------------
template <class ...Traits, class ...Args>
struct json_common<value<std::basic_string<Traits...>, Args...>, void>
{
    using target = typename value<std::basic_string<Traits...>, Args...>::target;

    template <class IteratorT>
    static IteratorT serialize(const target& value, IteratorT it)
    {
        return details::json_strokanizer::serialize(value.cbegin(), value.cend(), it);
    }
};

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_common<array<Args...>, void>
{
    using target = typename array<Args...>::target;
    using traits = typename array<Args...>::traits;
    using item_meta = typename array<Args...>::item_meta;

    template <class IteratorT>
    static IteratorT serialize(const target& container, IteratorT it)
    {
        using item_serializer = json_common<item_meta>;
        using char_type = typename iterator_traits<IteratorT>::value_type;

        *(it++) = char_type{ '[' };
        auto cur = traits::cbegin(container);
        auto end = traits::cend(container);
        if (cur == end) {
            *(it++) = char_type{ ']' };
            return it;
        }

        it = item_serializer::serialize(*(cur++), it);
        while (cur != end) {
            *(it++) = char_type{ ',' };
            it = item_serializer::serialize(*(cur++), it);
        }
        *(it++) = char_type{ ']' };
        return it;
    }
};

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_common<field<Args...>, void>
{
    using target = typename field<Args...>::target;
    using traits = typename field<Args...>::traits;
    using key_meta   = typename field<Args...>::key_meta;
    using value_meta = typename field<Args...>::value_meta;

    template <class IteratorT>
    static IteratorT serialize(const target& value, IteratorT it)
    {
        using key_serializer = typename json_common<key_meta>;
        using value_serializer = typename json_common<value_meta>;
        using char_type = typename iterator_traits<IteratorT>::value_type;

        //! TODO: implement code which ensures that serialization of the key is valid json.
        it = key_serializer::serialize(traits::key_ref(value), it);
        *(it++) = char_type{ ':' };
        return value_serializer::serialize(traits::value_ref(value), it);
    }
};

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_common<dictionary<Args...>, void>
{
    using target = typename dictionary<Args...>::target;
    using traits = typename dictionary<Args...>::traits;
    using field_meta = typename dictionary<Args...>::field_meta;

    template <class IteratorT>
    static IteratorT serialize(const target& container, IteratorT it)
    {
        using field_serializer = typename json_common<field_meta>;
        using char_type = typename iterator_traits<IteratorT>::value_type;

        *(it++) = char_type{ '{' };
        auto cur = traits::cbegin(container);
        auto end = traits::cend(container);
        if (cur == end) {
            *(it++) = char_type{ '}' };
            return it;
        }

        it = field_serializer::serialize(*(cur++), it);
        while (cur != end) {
            *(it++) = char_type{ ',' };
            it = field_serializer::serialize(*(cur++), it);
        }
        *(it++) = char_type{ '}' };
        return it;
    }
};

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_common<object<Args...>>
{
    using target = typename object<Args...>::target;
    using members = typename object<Args...>::members;

    template <class IteratorT>
    static IteratorT serialize(const target& object, IteratorT it)
    {
        using char_type = typename iterator_traits<IteratorT>::value_type;
        *(it++) = char_type{ '{' };
        it = serialize_members(object, members{}, it);
        *(it++) = char_type{ '}' };
        return it;
    }

private:
    template <class IteratorT, class Member, class Others>
    static IteratorT serialize_members(const target& object, fas::type_list<Member, Others>, IteratorT it)
    {
        using char_type = typename iterator_traits<IteratorT>::value_type;
        it = serialize_member(object, Member{}, it);
        *(it++) = char_type{ ',' };
        return serialize_members(object, Others{}, it);
    }

    template <class IteratorT, class Member>
    static IteratorT serialize_members(const target& object, fas::type_list<Member, fas::empty_list>, IteratorT it)
    {
        return serialize_member(object, Member{}, it);
    }

    template <class IteratorT>
    static IteratorT serialize_members(const target& object, fas::empty_list, IteratorT it)
    {
        return it;
    }

    template <class IteratorT, class Naming, class Object, class Member, Member Object::* m, class MetaType>
    static IteratorT serialize_member(const target& object, member<Naming, Object, Member, m, MetaType>, IteratorT it)
    {
        using char_type = typename iterator_traits<IteratorT>::value_type;

        *(it++) = char_type{ '"' };
        constexpr auto member_name = make_static_string<char_type>(Naming::value);
        it = std::copy(member_name.c_str(), member_name.c_str() + member_name.length(), it);
        *(it++) = char_type{ '"' };
        *(it++) = char_type{ ':' };

        using member_serializer = typename json_common<MetaType>;
        using member_traits = member_traits<Object, Member, m>;
        const auto& member_value = member_traits::member_ref(object);
        return member_serializer::serialize(member_value, it);
    }
};

} // namespace gts
