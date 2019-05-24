#pragma once
#include <iterator>
#include <type_traits>
#include <utility>
#include <forward_list>

// ----------------------------------------------------------------------------
namespace gts 
{

// ----------------------------------------------------------------------------
//                                  mpl helpers
// ----------------------------------------------------------------------------
namespace mpl 
{

// ----------------------------------------------------------------------------
template <class T>
struct void_t { using type = void; };

// ----------------------------------------------------------------------------
template <class T, class = void>
struct type_defined { enum : bool { value = false }; };

template <class T>
struct type_defined<T, typename void_t<T>::type> { enum : bool { value = true }; };

// ----------------------------------------------------------------------------
#define GTS_DEFINE_HAS_METHOD(method) \
    template <class T> \
    struct has_##method \
    { \
        struct no_method {}; \
         \
        template <class C> \
        static decltype(std::declval<C>().method(std::declval<typename C::value_type>())) test(void*); \
         \
        template <class C> \
        static no_method test(...); \
         \
        enum : bool { value = !std::is_same<decltype(test<T>(nullptr)), no_method>::value }; \
    };

GTS_DEFINE_HAS_METHOD(push_back)
GTS_DEFINE_HAS_METHOD(insert)

} // namespace mpl

// ----------------------------------------------------------------------------
//                                  array inserter
// ----------------------------------------------------------------------------
template <class T>
class array_inserter : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
    using value_type = T;
    using self_type = array_inserter<T>;

public:
    array_inserter()
        : m_cur(nullptr)
        , m_end(nullptr)
    {}

    array_inserter(T* data, std::size_t size)
        : m_cur(data)
        , m_end(m_cur + size)
    {}

    self_type& operator=(const value_type& value)
    {
        if (m_cur != m_end)
            *(m_cur++) = value;
        return (*this);
    }

    self_type& operator=(value_type&& value)
    {
        if (m_cur != m_end)
            *(m_cur++) = std::move(value);
        return (*this);
    }

    //! no-op methods
    self_type& operator*() { return (*this); }
    self_type& operator++() { return (*this); }
    self_type& operator++(int) { return (*this); }

private:
    T* m_cur;
    T* m_end;
};

// ----------------------------------------------------------------------------
//                                  array inserter
// ----------------------------------------------------------------------------
template <class ForwardList>
class forward_list_inserter : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
    using value_type = typename ForwardList::value_type;
    using self_type = forward_list_inserter<ForwardList>;

public:
    forward_list_inserter(ForwardList& container)
        : m_container(container)
    {}

    self_type& operator=(const value_type& value)
    {
        (void)m_container.insert_after(m_container.end(), value);
        return (*this);
    }

    self_type& operator=(value_type&& value)
    {
        (void)m_container.emplace_after(m_container.end(), std::move(value));
        return (*this);
    }

    //! no-op methods
    self_type& operator*() { return (*this); }
    self_type& operator++() { return (*this); }
    self_type& operator++(int) { return (*this); }

private:
    ForwardList& m_container;
};

} // namespace gts
