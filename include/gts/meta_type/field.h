#pragma once
#include <utility>
#include "common.h"

// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
//                                 field traits
// ----------------------------------------------------------------------------
template <class T, class = void>
struct field_traits;

// ----------------------------------------------------------------------------
template <class Key, class Value>
struct field_traits<std::pair<Key, Value>, void>
{
    using item_type  = std::pair<Key, Value>;
    using key_type   = typename std::remove_const<Key>::type;
    using value_type = Value;

    static key_type& key_ref(item_type& item) { return static_cast<key_type&>(item.first); };
    static const key_type& key_ref(const item_type& item) { return static_cast<const key_type&>(item.first); };

    static value_type& value_ref(item_type& item) { return static_cast<value_type&>(item.second); };
    static const value_type& value_ref(const item_type& item) { return static_cast<const value_type&>(item.second); };
};

// ----------------------------------------------------------------------------
//                              field meta type
// ----------------------------------------------------------------------------
template <class T, 
    class ValueMeta = typename meta_type<typename field_traits<T>::value_type>::value,
    class KeyMeta = typename meta_type<typename field_traits<T>::key_type>::value>
struct field
{
    using target = T;
    using traits = field_traits<T>;
    using key_meta   = KeyMeta;
    using value_meta = ValueMeta;
};

// ----------------------------------------------------------------------------
//                          default definitions
// ----------------------------------------------------------------------------
template <class Key, class Value>
struct meta_type<std::pair<Key, Value>>
{
    using value = typename field<std::pair<Key, Value>>;
};

} // namespace gts
