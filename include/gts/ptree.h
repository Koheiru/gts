#pragma once
#include <map>
#include <vector>
#include <boost/property_tree/ptree_fwd.hpp>
#include "common.h"
#include "error.h"

// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
template <class Key, class Data, class KeyCompare>
using basic_ptree = boost::property_tree::basic_ptree<Key, Data, KeyCompare>;

// ----------------------------------------------------------------------------
template <class J>
class ptree;

// ----------------------------------------------------------------------------
//                                  error
// ----------------------------------------------------------------------------
enum class ptree_code
{
    no_error,
    some_error
};

// ----------------------------------------------------------------------------
class ptree_error
{
public:
    explicit ptree_error(ptree_error* nested = nullptr)
        : m_code(ptree_code::no_error), m_nested(nested)
    {}

    explicit ptree_error(ptree_code code, ptree_error* nested = nullptr)
        : m_code(code), m_nested(nested)
    {}

    ptree_code code() const
    {
        return m_code;
    }

    operator bool() const
    {
        return (m_code != ptree_code::no_error);
    }

    const ptree_error* nested() const
    {
        return m_nested.get();
    }

private:
    ptree_code                   m_code;
    std::unique_ptr<ptree_error> m_nested;
};

// ----------------------------------------------------------------------------
template <class Key, class Data, class KeyCompare>
const basic_ptree<Key, Data, KeyCompare>& create_error(const basic_ptree<Key, Data, KeyCompare>& src, ptree_code code, ptree_error* error)
{
    if (error != nullptr)
    {
        std::unique_ptr<ptree_error> nested(new ptree_error());
        *nested = std::move(*error);
        *error = ptree_error(code, nested.release());
    }
    return ;
}

// ----------------------------------------------------------------------------
//                              value serializer
// ----------------------------------------------------------------------------
template <class T>
class ptree<typename value<T>>
{
public:
    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const T& value, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        dst.put_value(value);
        return dst;
    }

    template <class String, class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const String& name, const T& value, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        return serialize(value, dst.add_child(name, basic_ptree<Key, Data, KeyCompare>{}));
    }

    template <class Key, class Data, class KeyCompare>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize(T& value, const basic_ptree<Key, Data, KeyCompare>& src, ptree_error* error)
    {
        return src;
    }
};

// ----------------------------------------------------------------------------
//                              attribute serializer
// ----------------------------------------------------------------------------
template <class T>
class ptree<typename attribute<T>>
{
public:
    template <class String, class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const String& name, const T& value, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        dst.add_child("<xmlattr>." + std::string(name), basic_ptree<Key, Data, KeyCompare>{}).put_value(value);
        return dst;
    }

    template <class Key, class Data, class KeyCompare>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize(T& value, const basic_ptree<Key, Data, KeyCompare>& src)
    {
        return src;
    }
};

// ----------------------------------------------------------------------------
//                              array serializer
// ----------------------------------------------------------------------------
template <class T, class ...Args>
class ptree<typename array<T, Args...>>
{
public:
    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const T& values, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        using target      = typename array<T, Args...>::target;
        using item_traits = typename array<T, Args...>::item_traits;
        using item_naming = typename array<T, Args...>::item_naming;

        using serializer = typename ptree<item_traits>;
        for (auto it = array_traits<target>::cbegin(values); it != array_traits<target>::cend(values); ++it)
        {
            serializer::serialize(item_naming::value, *it, dst);
        }

        return dst;
    }

    template <class String, class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const String& name, const T& values, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        serialize(values, dst.add_child(name, basic_ptree<Key, Data, KeyCompare>{}));
        return dst;
    }

    template <class Key, class Data, class KeyCompare>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize(T& value, const basic_ptree<Key, Data, KeyCompare>& src)
    {
        return src;
    }
};

// ----------------------------------------------------------------------------
//                           dictionary serializer
// ----------------------------------------------------------------------------
template <class T, class ...Args>
class ptree<typename field<T, Args...>>
{
public:
    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const T& value, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        using target       = typename field<T, Args...>::target;
        using key_traits   = typename field<T, Args...>::key_traits;
        using value_traits = typename field<T, Args...>::value_traits;

        //! TODO: use key serializer instead of direct access.
        const auto key = Key(field_traits<target>::key_ref(value));

        using serializer = typename ptree<value_traits>;
        serializer::serialize(key, field_traits<target>::value_ref(value), dst);

        return dst;
    }

    template <class Key, class Data, class KeyCompare>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize(T& value, const basic_ptree<Key, Data, KeyCompare>& src)
    {
        return src;
    }
};

// ----------------------------------------------------------------------------
template <class T, class ...Args>
class ptree<typename dictionary<T, Args...>>
{
public:
    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const T& values, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        using target      = typename dictionary<T, Args...>::target;
        using item_traits = typename dictionary<T, Args...>::item_traits;

        using serializer = typename ptree<item_traits>;
        for (auto it = dictionary_traits<target>::cbegin(values); it != dictionary_traits<target>::cend(values); ++it)
        {
            serializer::serialize(*it, dst);
        }

        return dst;
    }

    template <class String, class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const String& name, const T& values, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        return serialize(values, dst.add_child(name, basic_ptree<Key, Data, KeyCompare>{}));
    }

    template <class Key, class Data, class KeyCompare>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize(T& value, const basic_ptree<Key, Data, KeyCompare>& src)
    {
        return src;
    }
};

// ----------------------------------------------------------------------------
//                              object serializer
// ----------------------------------------------------------------------------
template <class T, class Members>
class ptree<typename object<T, Members>>
{
public:
    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const T& value, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        return serialize_members(value, dst, Members());
    }

    template <class String, class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const String& name, const T& value, basic_ptree<Key, Data, KeyCompare>& dst)
    {
        return serialize(value, dst.add_child(name, basic_ptree<Key, Data, KeyCompare>{}));
    }

    template <class Key, class Data, class KeyCompare>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize(const basic_ptree<Key, Data, KeyCompare>& src, T& value, ptree_error* error = nullptr)
    {
        return deserialize_members(value, src, Members(), error);
    }

    template <class String, class Key, class Data, class KeyCompare>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize(const basic_ptree<Key, Data, KeyCompare>& src, const String& name, T& value, ptree_error* error = nullptr)
    {
        auto node = src.find(name);
        if (node == src.not_found())
            return create_error(src, ptree_code::some_error, error);
        return deserialize_members<>(node, Members(), value, error);
    }

private:
    template <class Key, class Data, class KeyCompare, class Member, class OtherMembers>
    static basic_ptree<Key, Data, KeyCompare>& serialize_members(const T& value, basic_ptree<Key, Data, KeyCompare>& dst, fas::type_list<Member, OtherMembers>)
    {
        serialize_member(value, dst, Member());
        serialize_members(value, dst, OtherMembers());
        return dst;
    }

    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize_members(const T& value, basic_ptree<Key, Data, KeyCompare>& dst, fas::empty_list)
    {
        return dst;
    }

    template <class Key, class Data, class KeyCompare, class T, class Member, Member T::* m, class Naming, class Traits>
    static basic_ptree<Key, Data, KeyCompare>& serialize_member(const T& value, basic_ptree<Key, Data, KeyCompare>& dst, const member<T, Member, m, Naming, Traits>&)
    {
        return ptree<Traits>::serialize(Naming::value, member<T, Member, m, Naming, Traits>::ref(value), dst);
    }

private:
    template <class Key, class Data, class KeyCompare, class Member, class OtherMembers>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize_members(const basic_ptree<Key, Data, KeyCompare>& src, fas::type_list<Member, OtherMembers>, T& value, ptree_error* error)
    {
        /*
        ptree_error sub_error;
        unserialize_member(value, src, Member(), &sub_error);
        if (!sub_error)
            unserialize_members(value, node, OtherMembers(), &sub_error);
        if (sub_error && (error != nullptr))
            error->swap(sub_error);
        */
        return src;
    }

    template <class Key, class Data, class KeyCompare>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize_members(const basic_ptree<Key, Data, KeyCompare>& src, fas::empty_list, T& value, ptree_error* error)
    {
        return src;
    }

    /*
    template <class Key, class Data, class KeyCompare, class T, class Member, Member T::* m, class Naming, class Traits>
    static const basic_ptree<Key, Data, KeyCompare>& deserialize_member(T& value, const basic_ptree<Key, Data, KeyCompare>& src, const member<T, Member, m, Naming, Traits>&, ptree_error* error)
    {
        auto node = src.find(Naming::value);
        if (node == src.not_found())
            return create_error(src, src, error_code::some_error, error);
        return ptree<Traits>::deserialize(member<T, Member, m, Naming, Traits>::ref(value), node, error);
    }
    */
};

} // namespace gts
