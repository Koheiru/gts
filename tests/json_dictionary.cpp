#include <boost/property_tree/json_parser.hpp>
#include <gtest/gtest.h>
#include <gts/json.h>

// ----------------------------------------------------------------------------
template <class T>
class JsonDictionarySuite : public ::testing::Test 
{
public:
    JsonDictionarySuite() = default;
    virtual ~JsonDictionarySuite() = default;
};

// ----------------------------------------------------------------------------
TYPED_TEST_SUITE_P(JsonDictionarySuite);

// ----------------------------------------------------------------------------
template <class C>
struct DictionaryHook;

template <template<class...> class C, class T, class ...Rest>
struct DictionaryHook<C<std::string, T, Rest...>>
{
    template <class U>
    using Container = C<std::string, U, Rest...>;
};

template <template<class...> class C, class T, class ...Rest>
struct DictionaryHook<C<std::pair<std::string, T>, Rest...>>
{
    template <class U>
    using Container = C<std::pair<std::string, U>, Rest...>;
};

// ----------------------------------------------------------------------------
static std::string FormatPtree(const boost::property_tree::ptree& tree)
{
    std::stringstream stream;
    boost::property_tree::write_json(stream, tree, false);
    return stream.str();
}

// ----------------------------------------------------------------------------
template <class Container>
static boost::property_tree::ptree SerializeByBoost(const Container& items)
{
    boost::property_tree::ptree tree;
    for (const auto& item : items)
    {
        boost::property_tree::ptree child{};
        child.put_value(item.second);
        tree.push_back(std::make_pair(item.first, child));
    }
    return tree;
}

// ----------------------------------------------------------------------------
template <class T>
static bool CheckT()
{
    return true;
}

// ----------------------------------------------------------------------------
template <class Container>
static boost::property_tree::ptree SerializeByGts(const Container& items)
{
    using meta_type = typename gts::meta_type<Container>::value;
    const auto isChecked = CheckT<meta_type>();

    using serializer = typename gts::json<meta_type>;
    const auto tree = serializer::serialize(items, boost::property_tree::ptree{});
    return tree;
}

// ----------------------------------------------------------------------------
TYPED_TEST_P(JsonDictionarySuite, SerializeStrings)
{
    using Container = DictionaryHook<TypeParam>::Container<std::string>;
    const Container items({ 
        { std::string("first"),  std::string("one") },
        { std::string("second"), std::string("two and three") },
        { std::string("third"),  std::string(" four ") } });
    const auto text = FormatPtree(SerializeByGts(items));
    std::cout << "JSON: " << text << std::endl;
    EXPECT_EQ(text, "{\"first\":\"one\",\"second\":\"two and three\",\"third\":\" four \"}\n");
}

// ----------------------------------------------------------------------------
TYPED_TEST_P(JsonDictionarySuite, SerializeDictionaries)
{
    using Dictionary = std::map<std::string, std::string>;
    using Container = DictionaryHook<TypeParam>::Container<Dictionary>;
    const Container items({
        { std::string("first"),  
            Dictionary({ { std::string("one"), std::string("1") } }) },
        { std::string("second"), 
            Dictionary({ { std::string("two and three"), std::string("2 & 3") } }) },
        { std::string("third"),  
            Dictionary({ { std::string("four"), std::string("4") } }) },
    });
    const auto text = FormatPtree(SerializeByGts(items));
    std::cout << "JSON: " << text << std::endl;
    EXPECT_EQ(text, "{\"first\":{\"one\":\"1\"},\"second\":{\"two and three\":\"2 & 3\"},\"third\":{\"four\":\"4\"}}\n");
}

// ----------------------------------------------------------------------------
REGISTER_TYPED_TEST_SUITE_P(JsonDictionarySuite, 
    SerializeStrings,
    SerializeDictionaries);

// ----------------------------------------------------------------------------
struct tag {};

using Containers = ::testing::Types<
    std::map<std::string, tag>, std::multimap<std::string, tag>, std::unordered_map<std::string, tag>,
    std::vector<std::pair<std::string, tag>>, std::deque<std::pair<std::string, tag>>, std::list<std::pair<std::string, tag>>, std::forward_list<std::pair<std::string, tag>>>;

INSTANTIATE_TYPED_TEST_SUITE_P(My, JsonDictionarySuite, Containers);
