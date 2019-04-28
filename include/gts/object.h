#pragma once
/*
#include <fas/type_list.hpp>
#include "common.h"

// ----------------------------------------------------------------------------
namespace gts {

/*
// ----------------------------------------------------------------------------
template <class T, class Members>
class serializer<object<T, Members>>
{
public:
    template <class Iterator>
    static Iterator serialize(const T& value, Iterator pos)
    {
        using CharT = typename ci_traits<Iterator>::char_type;

        *(pos++) = CharT{ '{' };
        pos = this->serialize_members(value, pos, Members());
        *(pos++) = CharT{ '}' };

        return pos;
    }

    template <class Iterator>
    static Iterator unserialize(T& value, Iterator pos, Iterator end, parse_error* error)
    {
        using CharT = typename ci_traits<Iterator>::char_type;

        if (pos == end)
            return make_parse_error(pos, end, error_code::some_error, error);

        return make_parse_error(pos, end, error_code::some_error, error);
    }

private:
    template <class Iterator, class Member, class MembersTail>
    Iterator serialize_members(const T& value, Iterator pos, fas::type_list<Member, MembersTail>)
    {
        using CharT = typename ci_traits<Iterator>::char_type;
        pos = this->serialize_member(value, pos, Member());
        *(pos++) = CharT{ ',' };
        return this->serialize_members(value, pos, MembersTail());
    }

    template <class Iterator, class Member>
    Iterator serialize_members(const T& value, Iterator pos, fas::type_list<Member, fas::empty_list>)
    {
        return this->serialize_member(value, pos, Member());
    }

    template <class Iterator>
    Iterator serialize_members(const T&, Iterator pos, fas::empty_list)
    {
        return pos;
    }

    template <class Iterator, typename N, typename G, typename M, M G::* m, typename W >
    Iterator serialize_member(const T& value, Iterator pos, const member<N, G, M, m, W>&)
    {
        using CharT = typename ci_traits<Iterator>::char_type;

        *(pos++)= CharT{ '"' };
        const char* name = member<N, G, M, m, W>::name();
        while (*name!= '\0') {
            *(end++) = CharT{ *(name++) };
        }
        *(pos++) = CharT{ '"' };
        *(pos++) = CharT{ ':' };

        using serializer = typename serializer<member<N, G, M, m, W>::traits>;
        return serializer::serialize(member<N, G, M, m, W>::ref(value), pos);
    }

};
*//*

} // namespace gts
*/