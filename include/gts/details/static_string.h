//
// The MIT License (MIT)
//
// Copyright (c) 2019 Yury Prostov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#pragma once
#include <array>
#include <string>
#include "index_sequence.h"

#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpedantic"
# if __GNUC__ < 5
//! GCC 4.9 Bug 61489 Wrong warning with -Wmissing-field-initializers.
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
# endif
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4351)
#endif // defined(_MSC_VER)

// ----------------------------------------------------------------------------
namespace gts
{

// ---------------------------------------------------------------------------
//                                  helpers
// ---------------------------------------------------------------------------
template <class DstT, std::size_t DstN, class SrcT, std::size_t SrcN>
constexpr DstT character_cvt(const SrcT(&str)[SrcN], std::size_t index)
{
    return (index >= SrcN) ? static_cast<DstT>(0) : static_cast<DstT>(str[index]);
}

// ---------------------------------------------------------------------------
template<class DstT, std::size_t DstN, class SrcT, std::size_t SrcN, std::size_t... Indexes>
constexpr std::array<DstT, DstN> make_literal_array_helper(const SrcT(&str)[SrcN], std_impl::index_sequence<Indexes...>)
{
    return{ { character_cvt<DstT, DstN>(str, Indexes)... } };
}

// ---------------------------------------------------------------------------
template<class DstT, std::size_t DstN, class SrcT, std::size_t SrcN>
constexpr std::array<DstT, DstN> make_literal_array(const SrcT(&str)[SrcN])
{
    static_assert(DstN >= SrcN, "Source string is too long");
    return make_literal_array_helper<DstT, DstN>(str, std_impl::make_index_sequence<DstN>{});
}

// ----------------------------------------------------------------------------
//                               static_string
// ----------------------------------------------------------------------------
template <class CharT, std::size_t Length>
class static_string
{
public:
    using value_type = CharT;

public:
    template <std::size_t Count>
    constexpr static_string(const CharT(&input)[Count])
        : m_data(make_literal_array<CharT, Length>(input))
        , m_length(Count - 1)
    {
        static_assert(Length >= Count, "Passed string is too long.");
    }

    template <std::size_t Count, class U = CharT, class = typename std::enable_if<2 <= sizeof(U)>::type>
    constexpr static_string(const char(&input)[Count])
        : m_data(make_literal_array<CharT, Length>(input))
        , m_length(Count - 1)
    {
        static_assert(Length >= Count, "Passed string is too long.");
    }

    constexpr const value_type* c_str() const
    {
        return m_data.data();
    }
    
    constexpr const value_type& operator[](std::size_t index) const
    {
        return m_data[index];
    }
    
    constexpr std::size_t length() const
    {
        return m_length;
    }
    
    constexpr bool empty() const
    {
        return (m_length == 0);
    }

private:
    const std::array<CharT, Length> m_data;
    const std::size_t m_length;
};

// ----------------------------------------------------------------------------
template <class CharT, std::size_t Length>
bool operator==(const static_string<CharT, Length>& lhs, const CharT* rhs)
{
    if (std::char_traits<CharT>::length(rhs) != lhs.length())
        return false;
    return (std::char_traits<CharT>::compare(rhs, lhs.c_str(), lhs.length()) == 0);
}

// ----------------------------------------------------------------------------
template <class CharT, std::size_t Length, std::size_t N>
bool operator==(const static_string<CharT, Length>& lhs, const CharT (&rhs)[N])
{
    if (N < lhs.length())
        return false;
    return (std::char_traits<CharT>::compare(static_cast<const CharT*>(rhs), lhs.c_str(), lhs.length()) == 0);
}

// ----------------------------------------------------------------------------
template <class CharT, class Traits, class Allocator, std::size_t Length>
bool operator==(const static_string<CharT, Length>& lhs, const std::basic_string<CharT, Traits, Allocator>& rhs)
{
    return (rhs.compare(lhs.c_str()) == 0);
}

// ----------------------------------------------------------------------------
template <class CharT, class InputT, std::size_t Length>
constexpr static_string<CharT, Length> make_static_string(const InputT(&input)[Length])
{
    return static_string<CharT, Length>(input);
}

} // namespace gts

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // defined(_MSC_VER)

#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
