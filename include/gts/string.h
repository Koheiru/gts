#pragma once
/*
#include <fas/type_list.hpp>
#include "common.h"
#include "error.h"

// ----------------------------------------------------------------------------
namespace gts {

// ----------------------------------------------------------------------------
template <class T>
class string_serializer
{
public:
    template <class Iterator>
    static Iterator serialize(T value, Iterator pos)
    {
        return pos;
    }

    template <class Iterator>
    static Iterator unserialize(T& value, Iterator pos, Iterator end, parse_error* error)
    {
        return make_parse_error(pos, end, error_code::some_error, error);
    }
};

} // namespace gts


/*
#include <string>
#include <type_traits>
#include <iterator>
#include <array>
#include <fas/type_list.hpp>

namespace serlib
{

// ---------------------------------------------------------------------------
namespace std_impl {

// ---------------------------------------------------------------------------
template <size_t... Ints>
struct index_sequence
{
    using type = index_sequence;
    using value_type = size_t;
    static constexpr std::size_t size() noexcept { return sizeof...(Ints); }
};

// --------------------------------------------------------------
namespace details {

    // --------------------------------------------------------------
    template <class Sequence1, class Sequence2>
    struct merge_and_renumber;

    // --------------------------------------------------------------
    template <size_t... I1, size_t... I2>
    struct merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>>
        : index_sequence<I1..., (sizeof...(I1)+I2)...>
    { };

} // namespace details

    // --------------------------------------------------------------

template <size_t N>
struct make_index_sequence
    : details::merge_and_renumber<typename make_index_sequence<N / 2>::type, typename make_index_sequence<N - N / 2>::type>
{ };

template<> struct make_index_sequence<0> : index_sequence<> { };
template<> struct make_index_sequence<1> : index_sequence<0> { };

} // namespace std_impl

// ----------------------------------------------------------------------------
//                          Additional stuff
// ----------------------------------------------------------------------------
template <class Iterator>
struct ci_traits
{
    using char_type = typename std::iterator_traits<Iterator>::value_type;
};

template <class T, class Traits>
struct ci_traits<std::ostreambuf_iterator<T, Traits>>
{
    using char_type = typename std::remove_cv<T>::type;
};

// ----------------------------------------------------------------------------
//                               string
// ----------------------------------------------------------------------------
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4351)
#endif // defined(_MSC_VER)

// ----------------------------------------------------------------------------
template <class DstT, size_t DstN, class SrcT, unsigned SrcN>
constexpr DstT character_cvt(const SrcT(&str)[SrcN], int index)
{
    return (index >= SrcN) ? static_cast<DstT>(0) : static_cast<DstT>(str[index]);
}

template<class DstT, size_t DstN, class SrcT, unsigned SrcN, std::size_t... Indexes>
constexpr std::array<DstT, DstN> make_literal_array_helper(const SrcT(&str)[SrcN], std_impl::index_sequence<Indexes...>)
{
    return{ { character_cvt<DstT, DstN>(str, Indexes)... } };
}

template<class DstT, size_t DstN, class SrcT, unsigned SrcN>
constexpr std::array<DstT, DstN> make_literal_array(const SrcT(&str)[SrcN])
{
    static_assert(DstN >= SrcN, "Source string is too long");
    return make_literal_array_helper<DstT, DstN>(str, std_impl::make_index_sequence<DstN>{});
}

// ----------------------------------------------------------------------------
template <class CharT, unsigned Length>
class string_literal
{
public:
    typedef CharT value_type;

public:
    template <unsigned Count>
    constexpr string_literal(const CharT(&value)[Count])
        : m_data(make_literal_array<CharT, Length>(value))
        , m_size(Count - 1)
    {
        static_assert(Length >= Count, "Passed string is too long");
    }

    template <unsigned Count, class U = CharT, class = typename std::enable_if<2 <= sizeof(U)>::type>
    constexpr string_literal(const char(&value)[Count])
        : m_data(make_literal_array<CharT, Length>(value))
        , m_size(Count - 1)
    {
        static_assert(Length >= Count, "Passed string is too long");
    }

    constexpr const value_type* c_str() const { return m_data.data(); }
    constexpr const value_type& operator[](size_t index) const { return m_data[index]; }
    constexpr const value_type* cbegin() const { return m_data.data(); }
    constexpr const value_type* cend() const { return m_data.data() + m_size; }
    constexpr size_t size() const { return m_size; }
    constexpr bool empty() const { return (m_size == 0); }

private:
    const std::array<CharT, Length> m_data;
    const size_t m_size;
};

template <class CharT, class Traits, class Allocator, unsigned Length>
inline bool operator==(const string_literal<CharT, Length>& lhs, const std::basic_string<CharT, Traits, Allocator>& rhs)
{
    return (rhs.compare(lhs.c_str()) == 0);
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // defined(_MSC_VER)


// ----------------------------------------------------------------------------
//                          Errors handling
// ----------------------------------------------------------------------------
enum class error_code
{
    no_error,
    some_error
};

class parse_error
{
public:
    parse_error() : m_code(error_code::no_error) {}
    parse_error(error_code ec) : m_code(ec) {}
    operator bool() const { return m_code != error_code::no_error; }

private:
    error_code m_code;
};

template <class Iterator>
Iterator make_parse_error(Iterator pos, Iterator end, error_code ec, parse_error* error)
{
    if (error != nullptr && !(*error)) {
        *error = parse_error(ec);
    }
    return end;
}

// ----------------------------------------------------------------------------
//                          Some predefinitions
// ----------------------------------------------------------------------------
template <class T>
struct value
{
    using target = T;
};

// ----------------------------------------------------------------------------
template <class N, class T, class Member, Member T::* member, class Traits = typename value<Member>>
struct member
{
    using object = T;
    using target = Member;
    using traits = Traits;

    static const char* name() { return N::operator()(); }
    static target* ptr(object* obj) { return static_cast<target*>(obj->*member); };
    static target& ref(object& obj) { return static_cast<target&>(obj.*member); };
    static const target* ptr(const object* obj) { return static_cast<const target*>(obj->*member); };
    static const target& ref(const object& obj) { return static_cast<const target&>(obj.*member); };
};

// ----------------------------------------------------------------------------
template <class ...Args>
using member_list = typename fas::type_list_n<Args...>::type;

// ----------------------------------------------------------------------------
template <class T, class Members>
struct object
{
    using target = T;
    using member_list = typename ::fas::normalize<Members>::type;
};

// ----------------------------------------------------------------------------
//                            Generic serializers
// ----------------------------------------------------------------------------
template <class T, class = void>
class serializer;

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

    template <class Iterator>
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
};

// ----------------------------------------------------------------------------
template <class T>
class serializer<value<T>, typename std::enable_if<std::is_floating_point<T>::value, void>::type>
{
public:
    template <class Iterator>
    static Iterator serialize(T value, Iterator pos)
    {
        return pos;
    }

    template <class Iterator>
    static Iterator unserialize(T& value, Iterator pos, Iterator end, parse_error* error)
    {
        return end;
    }
};

// ----------------------------------------------------------------------------
class string_serializer
{
public:
    template <class Iterator, class StringIterator>
    static Iterator serialize(StringIterator begin, StringIterator end, Iterator pos)
    {
        using CharT = typename ci_traits<Iterator>::char_type;
        using StrT  = typename ci_traits<StringIterator>::char_type;

        *(pos++) = CharT{ '"' };
        while (begin != end && *begin != StrT{ '\0' })
        {
            const std::char_traits<StrT>::int_type ich = std::char_traits<StrT>::to_int_type(*begin);
            if (ich < 32 || *begin == StrT{ '"' } || *begin == StrT{ '\\' } || *begin == StrT{ '/' })
            {
                switch (*begin)
                {
                    case StrT{ '"' } : *(pos++) = CharT{ '\\' }; *(pos++) = CharT{ '"' };  break;
                    case StrT{ '\\' }: *(pos++) = CharT{ '\\' }; *(pos++) = CharT{ '\\' }; break;
                    case StrT{ '/' } : *(pos++) = CharT{ '\\' }; *(pos++) = CharT{ '/' };  break;
                    case StrT{ '\t' }: *(pos++) = CharT{ '\\' }; *(pos++) = CharT{ '\t' }; break;
                    case StrT{ '\b' }: *(pos++) = CharT{ '\\' }; *(pos++) = CharT{ '\b' }; break;
                    case StrT{ '\r' }: *(pos++) = CharT{ '\\' }; *(pos++) = CharT{ '\r' }; break;
                    case StrT{ '\n' }: *(pos++) = CharT{ '\\' }; *(pos++) = CharT{ '\n' }; break;
                    case StrT{ '\f' }: *(pos++) = CharT{ '\\' }; *(pos++) = CharT{ '\f' }; break;
                    default: 
                    {
                        //...
                        throw std::exception("...");
                    }
                }
                ++begin;
            }
            else if (ich < 127)
            {
                *(pos++) = static_cast<CharT>(*(begin++));
            }
            else
            {
                //...
                throw std::exception("...");
            }
        }
        *(pos++) = CharT{ '"' };
        return pos;
    }

    template <class Iterator, class StringIterator>
    static Iterator unserialize(StringIterator dst, size_t capacity, Iterator pos, Iterator end, parse_error* error)
    {
        using CharT = typename ci_traits<Iterator>::char_type;
        using StrT  = typename ci_traits<StringIterator>::char_type;
        return end;
        /*
        if (begin == end)
            throw std::exception("...");
        
        if (*(begin++) != SrcT{ '"' })
            throw std::exception("...");

        while (begin != end && *begin != SrcT{ '"' })
        {
            if (*begin == SrcT{ '\\' })
            {
                if (++begin == end)
                    throw std::exception("...");

                switch (*begin)
                {
                    case SrcT{ '"' }:  *(dst++) = CharT{ '"' };  ++begin; break;
                    case SrcT{ '\\' }: *(dst++) = CharT{ '\\' }; ++begin; break;
                    case SrcT{ '/' }:  *(dst++) = CharT{ '/' };  ++begin; break;
                    case SrcT{ 't' }:  *(dst++) = CharT{ '\t' }; ++begin; break;
                    case SrcT{ 'b' }:  *(dst++) = CharT{ '\b' }; ++begin; break;
                    case SrcT{ 'r' }:  *(dst++) = CharT{ '\r' }; ++begin; break;
                    case SrcT{ 'n' }:  *(dst++) = CharT{ '\n' }; ++begin; break;
                    case SrcT{ 'f' }:  *(dst++) = CharT{ '\f' }; ++begin; break;
                    //case SrcT{ 'u' }:  beg = this->unserialize_uhex_(++beg, end, &vitr, n, e); break;
                    //case SrcT{ 'x' }:  beg = this->unserialize_xhex_(++beg, end, &vitr, n, e); break;
                    default:
                        throw std::exception("...");
                }
            }
            else
            {
                const std::char_traits<SrcT>::int_type ich = std::char_traits<SrcT>::to_int_type(*begin);
                if (ich < 127)
                {
                    *(dst++) = static_cast<CharT>(*(begin++))
                }
                else
                {
                    throw std::exception("...");
                    // ...
                }
            }
        }

        if (begin == end)
            throw std::exception("...");

        if (*(begin++) != SrcT{ '"' })
            throw std::exception("...");

        return begin;
        ////
    }
};

// ----------------------------------------------------------------------------
template <class CharT, class Traits>
class serializer<value<std::basic_string<CharT, Traits>>>
{
public:
    template <class Iterator>
    static Iterator serialize(const std::basic_string<CharT, Traits>& value, Iterator dst)
    {
        return string_serializer::serialize(value.cbegin(), value.cend(), dst);
    }

    template <class Iterator>
    static Iterator unserialize(std::basic_string<CharT, Traits>& value, Iterator begin, Iterator end, parse_error* error)
    {
        return string_serializer::unserialize(std::back_inserter(value), std::numeric_limits<size_t>::max(), begin, end, error);
    }
};

// ----------------------------------------------------------------------------
class json_error : public parse_error
{
public:
    json_error() : parse_error() {}
    json_error(error_code ec) : parse_error(ec) {}
};

// ----------------------------------------------------------------------------
template <class T, class J = typename value<T>>
struct json
{
    using target     = typename J::target;
    using serializer = typename serializer<J>;

    template <class Iterator>
    static Iterator serialize(const T& value, Iterator dst)
    {
        return serializer::serialize(value, dst);
    }

    template <class Iterator>
    static Iterator unserialize(T& value, Iterator begin, Iterator end, json_error* error = nullptr)
    {
        return serializer::unserialize(value, begin, end, static_cast<parse_error*>(error));
    }
};

} // namespace serlib
*/
