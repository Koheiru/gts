#pragma once
/*
#include "common.h"
#include "error.h"
#include "bool.h"
#include "integer.h"
#include "float.h"
//#include "string.h"
#include "object.h"

// ----------------------------------------------------------------------------
namespace gts {

// ----------------------------------------------------------------------------
using json_error = parse_error;

// ----------------------------------------------------------------------------
template <class T, class J = typename value<T>>
struct json
{
    using target     = typename J::target;
    using serializer = typename serializer<J>;

    template <class Iterator>
    static Iterator serialize(const T& value, Iterator dst)
    {
        return serializer::serialize(value, dst);
    }

    template <class Iterator>
    static Iterator unserialize(T& value, Iterator begin, Iterator end, json_error* error = nullptr)
    {
        return serializer::unserialize(value, begin, end, static_cast<parse_error*>(error));
    }
};

} // namespace gts
*/