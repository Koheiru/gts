#pragma once
#include <iterator>
#include <utility>
#include <fas/type_list.hpp>
#include <fas/functional.hpp>

// ----------------------------------------------------------------------------
#define GTS_NAME(FIELD) \
    struct n_##FIELD { \
        static constexpr const char* const value = #FIELD; \
    };

#define GTS_NAME_EX(FIELD, VALUE) \
    struct n_##FIELD { \
        static constexpr const char* const value = VALUE; \
    };

// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
//                                 MPL details
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
//                                 Array traits
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
        if (m_cur == m_end)
            throw std::out_of_range("attempt to insert more elements than expected");
        *(m_cur++) = value;
        return (*this);
    }

    self_type& operator=(value_type&& value)
    {
        if (m_cur == m_end)
            throw std::out_of_range("attempt to insert more elements than expected");
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
template <class Container, class = void>
struct array_read_traits;

template <class Container>
struct array_read_traits<Container, typename std::enable_if<mpl::type_defined<typename Container::const_iterator>::value>::type>
{
    using const_iterator = typename Container::const_iterator;
    static const_iterator cbegin(const Container& c) { return c.cbegin(); }
    static const_iterator cend(const Container& c) { return c.cend(); }
};

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

template <class Container>
struct array_write_traits<Container, typename std::enable_if<mpl::has_push_back<Container>::value>::type>
{
    using insert_iterator = std::back_insert_iterator<Container>;
    static insert_iterator inserter(Container& c) { return std::back_inserter(c); }
};

template <class Container>
struct array_write_traits<Container, typename std::enable_if<!mpl::has_push_back<Container>::value && mpl::has_insert<Container>::value>::type>
{
    using insert_iterator = std::insert_iterator<Container>;
    static insert_iterator inserter(Container& c) { return std::inserter(c, c.begin()); }
};

template <class T, std::size_t N>
struct array_write_traits<std::array<T, N>, void>
{
    using insert_iterator = array_inserter<T>;
    static insert_iterator inserter(std::array<T, N>& c) { insert_iterator(c.data(), c.size()); }
};

template <class T, std::size_t N>
struct array_write_traits<T[N], void>
{
    using insert_iterator = array_inserter<T>;
    static insert_iterator inserter(T (&c)[N]) { insert_iterator(&c[0], N); }
};

// -------------------------------------------------------
template <class Container, class = void>
struct array_info_traits
{
    using container_type = Container;
    using item_type      = typename Container::value_type;
};

template <class T, std::size_t N>
struct array_info_traits<T[N], void>
{
    using container_type = T[N];
    using item_type      = T;
};

// -------------------------------------------------------
template <class Container>
struct array_traits :
    public array_info_traits<Container>,
    public array_read_traits<Container>,
    public array_write_traits<Container>
{};

// ----------------------------------------------------------------------------
struct array_item_naming
{
    static constexpr const char* const value = "item";
};

// ----------------------------------------------------------------------------
//                                 Field traits
// ----------------------------------------------------------------------------
template <class Item, class = void>
struct field_traits;

// ----------------------------------------------------------------------------
template <class Key, class Value>
struct field_traits<std::pair<Key, Value>, void>
{
    using item_type  = std::pair<Key, Value>;
    using key_type   = Key;
    using value_type = Value;

    static key_type& key_ref(item_type& item) { return static_cast<key_type&>(item.first); };
    static const key_type& key_ref(const item_type& item) { return static_cast<const key_type&>(item.first); };

    static value_type& value_ref(item_type& item) { return static_cast<value_type&>(item.second); };
    static const value_type& value_ref(const item_type& item) { return static_cast<const value_type&>(item.second); };
};

// ----------------------------------------------------------------------------
//                              Dictionary traits
// ----------------------------------------------------------------------------
template <class Container, class = void>
struct dictionary_read_traits
{
    using const_iterator = typename Container::const_iterator;
    static const_iterator cbegin(const Container& c) { return c.cbegin(); }
    static const_iterator cend(const Container& c) { return c.cend(); }
};

// ----------------------------------------------------------------------------
template <class Container, class = void>
struct dictionary_write_traits
{
    using insert_iterator = std::insert_iterator<Container>;
    static insert_iterator inserter(Container& c) { return std::inserter(c, c.begin()); }
};

// ----------------------------------------------------------------------------
template <class Container, class = void>
struct dictionary_info_traits
{
    using container_type = Container;
    using item_type      = typename Container::value_type;
};

// ----------------------------------------------------------------------------
template <class Container>
struct dictionary_traits :
    public dictionary_info_traits<Container>,
    public dictionary_read_traits<Container>,
    public dictionary_write_traits<Container>
{};

} // namespace gts

// ----------------------------------------------------------------------------
//                             Meta-objects
// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
template <class T>
struct value
{
    using target = T;
};

// ----------------------------------------------------------------------------
template <class T>
struct attribute
{
    using target = T;
};

// ----------------------------------------------------------------------------
template <class Container, class ItemTraits = value<typename array_traits<Container>::item_type>, class ItemNaming = array_item_naming>
struct array
{
    using target      = Container;
    using item_traits = ItemTraits;
    using item_naming = ItemNaming;
};

// ----------------------------------------------------------------------------
template <class Item, class KeyTraits = value<typename field_traits<Item>::key_type>, class ValueTraits = value<typename field_traits<Item>::value_type>>
struct field
{
    using target       = Item;
    using key_traits   = KeyTraits;
    using value_traits = ValueTraits;
};

// ----------------------------------------------------------------------------
template <class Container, class ItemTraits = field<typename dictionary_traits<Container>::item_type>>
struct dictionary
{
    using target      = Container;
    using item_traits = ItemTraits;
};

// ----------------------------------------------------------------------------
template <class T, class Member, Member T::* m, class Naming, class Traits = typename value<Member>>
struct member
{
    using object = T;
    using target = Member;
    using naming = Naming;
    using traits = Traits;

    static target& ref(object& obj) { return static_cast<target&>(obj.*m); };
    static const target& ref(const object& obj) { return static_cast<const target&>(obj.*m); };
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

} // namespace gts
