#pragma once
#include "ptree_serializer.h"

// ----------------------------------------------------------------------------
namespace gts 
{

// ----------------------------------------------------------------------------
template <class MetaInfo>
struct json
{
    template <class Key, class Data, class KeyCompare>
    static boost::property_tree::basic_ptree<Key, Data, KeyCompare>& serialize(const typename MetaInfo::target& value, boost::property_tree::basic_ptree<Key, Data, KeyCompare>& node)
    {
        return json_ptree<MetaInfo>::serialize(value, node);
        //return boost::property_tree::basic_ptree<Key, Data, KeyCompare>{};
    }
};

} // namespace gts
