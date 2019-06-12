#pragma once
#include <algorithm>
#include <iterator>
#include <type_traits>

// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
template <class T>
struct iterator_traits
{
    using value_type = typename std::iterator_traits<T>::value_type;
};

// ----------------------------------------------------------------------------
template <class Container>
struct iterator_traits<std::back_insert_iterator<Container>>
{
    using value_type = typename Container::value_type;
};

// ----------------------------------------------------------------------------
template <class Container>
struct iterator_traits<std::front_insert_iterator<Container>>
{
    using value_type = typename Container::value_type;
};

} // namespace gts
