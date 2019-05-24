#include <boost/property_tree/json_parser.hpp>
#include <gtest/gtest.h>
#include <gts/json.h>

// ----------------------------------------------------------------------------
class JsonObjectSuite : public ::testing::Test 
{
public:
    JsonObjectSuite() = default;
    virtual ~JsonObjectSuite() = default;
};

// ----------------------------------------------------------------------------
static std::string FormatPtree(const boost::property_tree::ptree& tree)
{
    std::stringstream stream;
    boost::property_tree::write_json(stream, tree, false);
    return stream.str();
}

// ----------------------------------------------------------------------------
struct TestItem
{
    std::uint32_t   id;
    std::string     name;
    bool            enabled;
    double          value;
};

GTS_OBJECT(TestItem,
    GTS_MEMBER(id,      "id")
    GTS_MEMBER(name,    "name")
    GTS_MEMBER(enabled, "enabled")
    GTS_MEMBER(value,   "value")
)

// ----------------------------------------------------------------------------
TEST_F(JsonObjectSuite, SerializeItem)
{
    TestItem item{ 32, std::string("item 32"), true, 12.5 };

    using meta_type = meta_type_TestItem::value;
    using serializer = gts::json<meta_type>;
    const auto tree = serializer::serialize(item, boost::property_tree::ptree{});

    const auto text = FormatPtree(tree);
    std::cout << "JSON: " << text << std::endl;
    EXPECT_EQ(text, "{\"id\":\"32\",\"name\":\"item 32\",\"enabled\":\"true\",\"value\":\"12.5\"}\n");
}

// ----------------------------------------------------------------------------
TEST_F(JsonObjectSuite, SerializeItemList)
{
    std::vector<TestItem> items = {
        TestItem{ 32, std::string("item 32"), true,  12.5 },
        TestItem{ 64, std::string("item 64"), false, -2.5 },
        TestItem{ 72, std::string("item 72"), true,  79.5 },
    };

    boost::property_tree::ptree tree;
    auto& children = tree.put_child("items", boost::property_tree::ptree{});

    using meta_type = gts::array<decltype(items), meta_type_TestItem::value>;
    using serializer = gts::json<meta_type>;
    (void)serializer::serialize(items, children);

    const auto text = FormatPtree(tree);
    std::cout << "JSON: " << text << std::endl;
    EXPECT_EQ(text, "{\"items\":["
        "{\"id\":\"32\",\"name\":\"item 32\",\"enabled\":\"true\",\"value\":\"12.5\"},"
        "{\"id\":\"64\",\"name\":\"item 64\",\"enabled\":\"false\",\"value\":\"-2.5\"},"
        "{\"id\":\"72\",\"name\":\"item 72\",\"enabled\":\"true\",\"value\":\"79.5\"}]}\n");
}

// ----------------------------------------------------------------------------
TEST_F(JsonObjectSuite, SerializeItemMap)
{
    std::map<std::string, TestItem> items ={
        { std::string("item 32"), TestItem{ 32, std::string("item 32"), true,  12.5 } },
        { std::string("item 64"), TestItem{ 64, std::string("item 64"), false, -2.5 } },
        { std::string("item 72"), TestItem{ 72, std::string("item 72"), true,  79.5 } },
    };

    using meta_type = gts::dictionary<decltype(items), gts::field<decltype(items)::value_type, meta_type_TestItem::value>>;
    using serializer = gts::json<meta_type>;
    const auto tree = serializer::serialize(items, boost::property_tree::ptree{});

    const auto text = FormatPtree(tree);
    std::cout << "JSON: " << text << std::endl;
    EXPECT_EQ(text, "{"
        "\"item 32\":{\"id\":\"32\",\"name\":\"item 32\",\"enabled\":\"true\",\"value\":\"12.5\"},"
        "\"item 64\":{\"id\":\"64\",\"name\":\"item 64\",\"enabled\":\"false\",\"value\":\"-2.5\"},"
        "\"item 72\":{\"id\":\"72\",\"name\":\"item 72\",\"enabled\":\"true\",\"value\":\"79.5\"}}\n");
}

