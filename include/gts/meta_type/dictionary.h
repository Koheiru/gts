#pragma once
#include <iterator>
#include <utility>
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include "common.h"
#include "field.h"
#include "array.h"

// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
//                            dictionary traits
// ----------------------------------------------------------------------------
template <class Container, class = void>
struct dictionary_read_traits : array_read_traits<Container>
{};

// ----------------------------------------------------------------------------
template <class Container, class = void>
struct dictionary_write_traits : array_write_traits<Container>
{};

// ----------------------------------------------------------------------------
template <class Container, class = void>
struct dictionary_info_traits
{
    using container_type = Container;
    using field_type     = typename Container::value_type;
};

// ----------------------------------------------------------------------------
template <class Container>
struct dictionary_traits :
    dictionary_info_traits<Container>,
    dictionary_read_traits<Container>,
    dictionary_write_traits<Container>
{};

// ----------------------------------------------------------------------------
//                            dictionary meta type
// ----------------------------------------------------------------------------
template <class Container, class FieldMeta = typename meta_type<typename dictionary_traits<Container>::field_type>::value>
struct dictionary
{
    using target = Container;
    using traits = dictionary_traits<Container>;
    using field_meta = FieldMeta;
};

// ----------------------------------------------------------------------------
//                             default definitions
// ----------------------------------------------------------------------------
template <class K, class V, std::size_t N>
struct meta_type<std::array<std::pair<K, V>, N>>
{
    using value = typename dictionary<std::array<std::pair<K, V>, N>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Alloc>
struct meta_type<std::vector<std::pair<K, V>, Alloc>>
{
    using value = typename dictionary<std::vector<std::pair<K, V>, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Alloc>
struct meta_type<std::deque<std::pair<K, V>, Alloc>>
{
    using value = typename dictionary<std::deque<std::pair<K, V>, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Alloc>
struct meta_type<std::list<std::pair<K, V>, Alloc>>
{
    using value = typename dictionary<std::list<std::pair<K, V>, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Alloc>
struct meta_type<std::forward_list<std::pair<K, V>, Alloc>>
{
    using value = typename dictionary<std::forward_list<std::pair<K, V>, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Compare, class Alloc>
struct meta_type<std::set<std::pair<K, V>, Compare, Alloc>>
{
    using value = typename dictionary<std::set<std::pair<K, V>, Compare, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Compare, class Alloc>
struct meta_type<std::multiset<std::pair<K, V>, Compare, Alloc>>
{
    using value = typename dictionary<std::multiset<std::pair<K, V>, Compare, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Hash, class Pred, class Alloc>
struct meta_type<std::unordered_set<std::pair<K, V>, Hash, Pred, Alloc>>
{
    using value = typename dictionary<std::unordered_set<std::pair<K, V>, Hash, Pred, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Hash, class Pred, class Alloc>
struct meta_type<std::unordered_multiset<std::pair<K, V>, Hash, Pred, Alloc>>
{
    using value = typename dictionary<std::unordered_multiset<std::pair<K, V>, Hash, Pred, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Compare, class Alloc>
struct meta_type<std::map<K, V, Compare, Alloc>>
{
    using value = typename dictionary<std::map<K, V, Compare, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Compare, class Alloc>
struct meta_type<std::multimap<K, V, Compare, Alloc>>
{
    using value = typename dictionary<std::multimap<K, V, Compare, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Hash, class Pred, class Alloc>
struct meta_type<std::unordered_map<K, V, Hash, Pred, Alloc>>
{
    using value = typename dictionary<std::unordered_map<K, V, Hash, Pred, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class K, class V, class Hash, class Pred, class Alloc>
struct meta_type<std::unordered_multimap<K, V, Hash, Pred, Alloc>>
{
    using value = typename dictionary<std::unordered_multimap<K, V, Hash, Pred, Alloc>>;
};

} // namespace gts
