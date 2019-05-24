#pragma once
#include "common.h"

// ----------------------------------------------------------------------------
namespace gts
{
    
// ----------------------------------------------------------------------------
//                                member traits
// ----------------------------------------------------------------------------
template <class Object, class Member, Member Object::* member>
struct member_traits
{
    using member_type = Member;
    
    static Member& member_ref(Object& object) { return static_cast<Member&>(object.*member); };
    static const Member& member_ref(const Object& object) { return static_cast<const Member&>(object.*member); };    
};

// ----------------------------------------------------------------------------
//                              member meta type
// ----------------------------------------------------------------------------
template <class Naming, class Object, class Member, Member Object::* m,
    class MemberMeta = typename meta_type<typename member_traits<Object, Member, m>::member_type>::value>
struct member
{
    using object = Object;
    using target = Member;
    using naming = Naming;
    using traits = member_traits<Object, Member, m>;
    using member_meta = MemberMeta;
};

} // namespace gts
