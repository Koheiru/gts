#pragma once
#include <boost/preprocessor.hpp>
#include <fas/type_list.hpp>
#include <fas/functional.hpp>
#include "common.h"
#include "member.h"

// ----------------------------------------------------------------------------
#define GTS_NAME(field) \
    struct BOOST_PP_CAT(n_, field) { \
        static constexpr const char* const value = #field; \
    };

#define GTS_NAME_EX(field, name) \
    struct BOOST_PP_CAT(n_, field) { \
        static constexpr const char* const value = name; \
    };

// ----------------------------------------------------------------------------
#define GTS_DECLARE_NAME_HOLDERS(r, data, index, item) GTS_DECLARE_NAME_HOLDER(BOOST_PP_MOD(index, 2))(item)
#define GTS_DECLARE_NAME_HOLDER(value) BOOST_PP_CAT(GTS_DECLARE_NAME_HOLDER_, value)
#define GTS_DECLARE_NAME_HOLDER_1(item) /* nothing */
#define GTS_DECLARE_NAME_HOLDER_0(item) GTS_NAME_EX(BOOST_PP_TUPLE_ELEM(2, 0, item), BOOST_PP_TUPLE_ELEM(2, 1, item))

#define GTS_DECLATE_META_TYPE(item) BOOST_PP_CAT(GTS_DECLATE_META_TYPE_, BOOST_PP_TUPLE_SIZE(item))(item)
#define GTS_DECLATE_META_TYPE_2(item) /* nothing */
#define GTS_DECLATE_META_TYPE_3(item) , BOOST_PP_TUPLE_ELEM(3, 2, item)

#define GTS_DECLARE_TYPES(r, object, index, item) GTS_DECLARE_TYPE(index)(object, item)
#define GTS_DECLARE_TYPE(index) BOOST_PP_CAT(GTS_DECLARE_TYPE_, BOOST_PP_MOD(index, 2))
#define GTS_DECLARE_TYPE_0(object, item) /* nothing */
#define GTS_DECLARE_TYPE_1(object, item) \
    gts::member<BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(item), 0, item), object, \
        decltype(BOOST_PP_CAT(object ## ::, BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(item), 1, item))), \
        BOOST_PP_CAT(& ## object ## ::, BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(item), 1, item)) \
        GTS_DECLATE_META_TYPE(item)>, 

// ----------------------------------------------------------------------------
#define GTS_MEMBER(member, name, ...) \
    ((member, name))((BOOST_PP_CAT(n_, member), member, __VA_ARGS__))

#define GTS_OBJECT(type, members) \
    struct BOOST_PP_CAT(meta_type_, type) { \
        BOOST_PP_SEQ_FOR_EACH_I(GTS_DECLARE_NAME_HOLDERS, _, members) \
        using value = gts::object<type, gts::member_list<BOOST_PP_SEQ_FOR_EACH_I(GTS_DECLARE_TYPES, type, members) fas::empty_list>>; \
    };

//#define GTS_OBJECT(TYPE, MEMBERS) \ 
//    struct meta_type_##TYPE { \
//        BOOST_PP_SEQ_FOR_EACH_I(GTS_DECLARE_NAME_HOLDERS, _, MEMBERS) \
//        using value = gts::object<TYPE, gts::member_list<BOOST_PP_SEQ_FOR_EACH_I(GTS_DECLARE_TYPES, TYPE, MEMBERS) fas::empty_list>>; \
//    };
    

// ----------------------------------------------------------------------------
namespace gts
{
    
// ----------------------------------------------------------------------------
template <class ...Args>
struct object;

// ----------------------------------------------------------------------------
template <class ...Args>
struct member_list : fas::type_list_n<Args...>
{
    using type = typename fas::type_list_n<Args...>::type;
};

// ----------------------------------------------------------------------------
//                            object meta type
// ----------------------------------------------------------------------------
template <class Object, class ...Members>
struct object<Object, member_list<Members...>>
{
    using target = Object;
    using members = typename member_list<Members...>::type;
};

} // namespace gts
