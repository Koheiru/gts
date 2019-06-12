#pragma once
#include "serializer_ptree.h"
#include "serializer_common.h"

// ----------------------------------------------------------------------------
namespace gts 
{

// ----------------------------------------------------------------------------
template <class MetaInfo>
struct json
{
    template <class Iterator>
    static Iterator serialize(const typename MetaInfo::target& value, Iterator it)
    {
        return json_common<MetaInfo>::serialize(value, it);
    }

    template <class Key, class Data, class KeyCompare>
    static boost::property_tree::basic_ptree<Key, Data, KeyCompare>& serialize(const typename MetaInfo::target& value, boost::property_tree::basic_ptree<Key, Data, KeyCompare>& node)
    {
        return json_ptree<MetaInfo>::serialize(value, node);
    }
};

} // namespace gts
