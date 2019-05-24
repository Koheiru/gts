#pragma once
#include <iterator>
#include <utility>
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <unordered_set>
#include "common.h"
#include "helpers.h"

// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
//                                array traits
// ----------------------------------------------------------------------------
template <class Container, class = void>
struct array_read_traits;

// ----------------------------------------------------------------------------
template <class Container>
struct array_read_traits<Container, typename std::enable_if<mpl::type_defined<typename Container::const_iterator>::value>::type>
{
    using const_iterator = typename Container::const_iterator;
    static const_iterator cbegin(const Container& c) { return c.cbegin(); }
    static const_iterator cend(const Container& c) { return c.cend(); }
};

// ----------------------------------------------------------------------------
template <class T, std::size_t N>
struct array_read_traits<T[N], void>
{
    using const_iterator = const T*;
    static const_iterator cbegin(const T (&c)[N]) { return &c[0]; }
    static const_iterator cend(const T (&c)[N]) { return &c[0] + N; }
};

// ----------------------------------------------------------------------------
template <class Container, class = void>
struct array_write_traits;

// ----------------------------------------------------------------------------
template <class Container>
struct array_write_traits<Container, typename std::enable_if<mpl::has_push_back<Container>::value>::type>
{
    using insert_iterator = std::back_insert_iterator<Container>;
    static insert_iterator inserter(Container& c) { return std::back_inserter(c); }
};

// ----------------------------------------------------------------------------
template <class Container>
struct array_write_traits<Container, typename std::enable_if<!mpl::has_push_back<Container>::value && mpl::has_insert<Container>::value>::type>
{
    using insert_iterator = std::insert_iterator<Container>;
    static insert_iterator inserter(Container& c) { return std::inserter(c, c.end()); }
};

// ----------------------------------------------------------------------------
template <class T, std::size_t N>
struct array_write_traits<std::array<T, N>, void>
{
    using insert_iterator = array_inserter<T>;
    static insert_iterator inserter(std::array<T, N>& c) { insert_iterator(c.data(), c.size()); }
};

// ----------------------------------------------------------------------------
template <class T, std::size_t N>
struct array_write_traits<T[N], void>
{
    using insert_iterator = array_inserter<T>;
    static insert_iterator inserter(T (&c)[N]) { insert_iterator(&c[0], N); }
};

// ----------------------------------------------------------------------------
template <class T, class Alloc>
struct array_write_traits<std::forward_list<T, Alloc>, void>
{
    using insert_iterator = forward_list_inserter<std::forward_list<T, Alloc>>;
    static insert_iterator inserter(std::forward_list<T, Alloc>& c) { insert_iterator(c); }
};

// ----------------------------------------------------------------------------
template <class Container, class = void>
struct array_info_traits
{
    using container_type = Container;
    using item_type      = typename Container::value_type;
};

// ----------------------------------------------------------------------------
template <class T, std::size_t N>
struct array_info_traits<T[N], void>
{
    using container_type = T[N];
    using item_type      = T;
};

// ----------------------------------------------------------------------------
template <class Container>
struct array_traits :
    array_info_traits<Container>,
    array_read_traits<Container>,
    array_write_traits<Container>
{};

// ----------------------------------------------------------------------------
//                            array meta type
// ----------------------------------------------------------------------------
template <class Container, class ItemMeta = typename meta_type<typename array_traits<Container>::item_type>::value>
struct array
{
    using target = Container;
    using traits = array_traits<Container>;
    using item_meta = ItemMeta;
};

// ----------------------------------------------------------------------------
//                          default definitions
// ----------------------------------------------------------------------------
template <class T, std::size_t N>
struct meta_type<std::array<T, N>>
{
    using value = typename array<std::array<T, N>>;
};

// ----------------------------------------------------------------------------
template <class T, class Alloc>
struct meta_type<std::vector<T, Alloc>>
{
    using value = typename array<std::vector<T, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class T, class Alloc>
struct meta_type<std::deque<T, Alloc>>
{
    using value = typename array<std::deque<T, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class T, class Alloc>
struct meta_type<std::list<T, Alloc>>
{
    using value = typename array<std::list<T, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class T, class Alloc>
struct meta_type<std::forward_list<T, Alloc>>
{
    using value = typename array<std::forward_list<T, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class T, class Compare, class Alloc>
struct meta_type<std::set<T, Compare, Alloc>>
{
    using value = typename array<std::set<T, Compare, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class T, class Compare, class Alloc>
struct meta_type<std::multiset<T, Compare, Alloc>>
{
    using value = typename array<std::multiset<T, Compare, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class T, class Hash, class Pred, class Alloc>
struct meta_type<std::unordered_set<T, Hash, Pred, Alloc>>
{
    using value = typename array<std::unordered_set<T, Hash, Pred, Alloc>>;
};

// ----------------------------------------------------------------------------
template <class T, class Hash, class Pred, class Alloc>
struct meta_type<std::unordered_multiset<T, Hash, Pred, Alloc>>
{
    using value = typename array<std::unordered_multiset<T, Hash, Pred, Alloc>>;
};

} // namespace gts
