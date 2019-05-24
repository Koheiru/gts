#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include "../meta_type.h"

// ----------------------------------------------------------------------------
namespace gts
{

// ----------------------------------------------------------------------------
using namespace boost::property_tree;

// ----------------------------------------------------------------------------
template <class MetaInfo>
struct json_ptree;

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_ptree<value<Args...>>
{
    using target = typename value<Args...>::target;

    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const target& data, basic_ptree<Key, Data, KeyCompare>& node)
    {
        node.put_value(data);
        return node;
    }
};

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_ptree<array<Args...>>
{
    using target = typename array<Args...>::target;
    using traits = typename array<Args...>::traits;
    using item_meta = typename array<Args...>::item_meta;

    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const target& data, basic_ptree<Key, Data, KeyCompare>& node)
    {
        using item_serializer = json_ptree<item_meta>;
        for (auto it = traits::cbegin(data); it != traits::cend(data); ++it)
        {
            basic_ptree<Key, Data, KeyCompare> child{};
            (void)item_serializer::serialize(*it, child);
            node.push_back(std::make_pair(Key{}, child));
        }
        return node;
    }
};

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_ptree<field<Args...>>
{
    using target = typename field<Args...>::target;
    using traits = typename field<Args...>::traits;
    using key_meta   = typename field<Args...>::key_meta;
    using value_meta = typename field<Args...>::value_meta;

    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const target& data, basic_ptree<Key, Data, KeyCompare>& node)
    {
        using value_serializer = typename json_ptree<value_meta>;
        auto& child = node.add_child(Key{ traits::key_ref(data) }, basic_ptree<Key, Data, KeyCompare>{});
        value_serializer::serialize(traits::value_ref(data), child);
        return node;
    }
};

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_ptree<dictionary<Args...>>
{
    using target = typename dictionary<Args...>::target;
    using traits = typename dictionary<Args...>::traits;
    using field_meta = typename dictionary<Args...>::field_meta;

    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const target& data, basic_ptree<Key, Data, KeyCompare>& node)
    {
        using field_serializer = typename json_ptree<field_meta>;
        for (auto it = traits::cbegin(data); it != traits::cend(data); ++it)
        {
            field_serializer::serialize(*it, node);
        }
        return node;
    }
};

// ----------------------------------------------------------------------------
template <class ...Args>
struct json_ptree<object<Args...>>
{
    using target = typename object<Args...>::target;
    using members = typename object<Args...>::members;

    template <class Key, class Data, class KeyCompare>
    static basic_ptree<Key, Data, KeyCompare>& serialize(const target& object, basic_ptree<Key, Data, KeyCompare>& node)
    {
        serialize_members(object, members{}, node);
        return node;
    }

private:
    template <class Node, class Member, class Others>
    static void serialize_members(const target& object, fas::type_list<Member, Others>, Node& node)
    {
        serialize_member(object, Member{}, node);
        serialize_members(object, Others{}, node);
    }

    template <class Node>
    static void serialize_members(const target& object, fas::empty_list, Node& node)
    {}

    template <class Key, class Data, class KeyCompare, class Naming, class Object, class Member, Member Object::* m, class MetaType>
    static void serialize_member(const target& object, member<Naming, Object, Member, m, MetaType>, basic_ptree<Key, Data, KeyCompare>& node)
    {
        constexpr const char* const member_name = Naming::value;
        auto& member_node = node.add_child(member_name, basic_ptree<Key, Data, KeyCompare>{});

        using member_traits = member_traits<Object, Member, m>;
        const auto& member_value = member_traits::member_ref(object);

        using member_serializer = typename json_ptree<MetaType>;
        member_serializer::serialize(member_value, member_node);
    }
};

} // namespace gts
