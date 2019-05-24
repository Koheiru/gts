#include <boost/property_tree/json_parser.hpp>
#include <gtest/gtest.h>
#include <gts/json.h>

// ----------------------------------------------------------------------------
template <class T>
class JsonArraySuite : public ::testing::Test 
{
public:
    JsonArraySuite() = default;
    virtual ~JsonArraySuite() = default;
};

// ----------------------------------------------------------------------------
TYPED_TEST_SUITE_P(JsonArraySuite);

// ----------------------------------------------------------------------------
template <class C>
struct ArrayHook;

template <template<class...> class C, class T, class ...Rest>
struct ArrayHook<C<T, Rest...>>
{
    template <class U>
    using Container = C<U>;
};

// ----------------------------------------------------------------------------
static std::string FormatPtree(const boost::property_tree::ptree& tree)
{
    std::stringstream stream;
    boost::property_tree::write_json(stream, tree, false);
    return stream.str();
}

// ----------------------------------------------------------------------------
template <class T>
static bool CheckT()
{
    return true;
}

// ----------------------------------------------------------------------------
template <class Container>
static boost::property_tree::ptree SerializeByGts(const std::string& name, const Container& items)
{
    boost::property_tree::ptree tree;
    auto& children = tree.put_child(name, boost::property_tree::ptree{});

    using meta_type = typename gts::meta_type<Container>::value;
    const auto isChecked = CheckT<meta_type>();

    using serializer = typename gts::json<meta_type>;
    (void)serializer::serialize(items, children);
    return tree;
}

// ----------------------------------------------------------------------------
TYPED_TEST_P(JsonArraySuite, SerializeBoolean) 
{
    using Container = ArrayHook<TypeParam>::Container<bool>;
    const Container values({ true, true, false, false, true });
    const auto text = FormatPtree(SerializeByGts(std::string("array"), values));
    EXPECT_EQ(text, "{\"array\":[\"true\",\"true\",\"false\",\"false\",\"true\"]}\n");
}

// ----------------------------------------------------------------------------
TYPED_TEST_P(JsonArraySuite, SerializeUint32)
{
    using Container = ArrayHook<TypeParam>::Container<uint32_t>;
    const Container values({ 456, 789165, 001, std::numeric_limits<uint32_t>::max() });
    const auto text = FormatPtree(SerializeByGts(std::string("array"), values));
    EXPECT_EQ(text, "{\"array\":[\"456\",\"789165\",\"1\",\"4294967295\"]}\n");
}

// ----------------------------------------------------------------------------
TYPED_TEST_P(JsonArraySuite, SerializeStrings)
{
    using Container = ArrayHook<TypeParam>::Container<std::string>;
    const Container values({ std::string("one"), std::string("two and three"), std::string(" four ") });
    const auto text = FormatPtree(SerializeByGts(std::string("array"), values));
    EXPECT_EQ(text, "{\"array\":[\"one\",\"two and three\",\" four \"]}\n");
}

// ----------------------------------------------------------------------------
REGISTER_TYPED_TEST_SUITE_P(JsonArraySuite, 
    SerializeBoolean,
    SerializeUint32,
    SerializeStrings);

// ----------------------------------------------------------------------------
using tag = uint32_t;

using SequentialContainers = ::testing::Types<
    std::vector<tag>, std::list<tag>, std::deque<tag>, std::forward_list<tag>>;

INSTANTIATE_TYPED_TEST_SUITE_P(My, JsonArraySuite, SequentialContainers);
