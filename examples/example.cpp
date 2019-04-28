#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <boost/property_tree/xml_parser.hpp>
#include <gts/ptree.h>

// ----------------------------------------------------------------------------
//                              Complex data
// ----------------------------------------------------------------------------
struct Test
{
    uint32_t    id;
    std::string name;
    bool        enabled;
};

template <class Stream>
Stream& operator<<(Stream& stream, const Test& obj)
{
    stream << "{ " 
        "id: " << obj.id << ", " <<
        "name: \"" << obj.name << "\", " <<
        "enabled: " << (obj.enabled ? "true" : "false") <<" }";
    return stream;
}

GTS_NAME_EX(id, "ID");
GTS_NAME_EX(name, "Name");
GTS_NAME_EX(enabled, "Enabled");

using TestMeta = 
    gts::object<Test, 
        gts::member_list<
            gts::member<Test, decltype(Test::id),      &Test::id,      n_id>,
            gts::member<Test, decltype(Test::name),    &Test::name,    n_name>,
            gts::member<Test, decltype(Test::enabled), &Test::enabled, n_enabled, gts::attribute<decltype(Test::enabled)>>
        >>;

// ----------------------------------------------------------------------------
struct TestSuite
{
    std::vector<Test> tests;
    std::array<int, 3> indexes;
    std::map<std::string, std::string> infos;
};

template <class Stream>
Stream& operator<<(Stream& stream, const TestSuite& obj)
{
    stream << "{ tests: [ ";
    for (size_t i = 0; i < obj.tests.size(); ++i)
        stream << obj.tests[i] << (i < obj.tests.size() - 1 ? ", " : " ");
    stream << "], indexes: [";
    for (size_t i = 0; i < obj.indexes.size(); ++i)
        stream << obj.indexes[i] << (i < obj.indexes.size() - 1 ? ", " : " ");
    stream << "], infos: { ";
    size_t count = 0;
    for (const auto& item : obj.infos)
        stream << item.first << ": " << item.second << (count++ < obj.infos.size() - 1 ? ", " : " ");
    stream << "} }";
    return stream;
}

GTS_NAME_EX(tests, "Tests");
//GTS_NAME_EX(suits, "Suits");
GTS_NAME_EX(indexes, "Indexes");
GTS_NAME_EX(infos, "Informations");
GTS_NAME_EX(tests_item, "TestItem");

using TestSuiteMeta = 
    gts::object<TestSuite,
        gts::member_list<
            gts::member<TestSuite, decltype(TestSuite::tests),   &TestSuite::tests,   n_tests,   gts::array<decltype(TestSuite::tests), TestMeta, n_tests_item>>,
            gts::member<TestSuite, decltype(TestSuite::indexes), &TestSuite::indexes, n_indexes, gts::array<decltype(TestSuite::indexes)>>,
            gts::member<TestSuite, decltype(TestSuite::infos),   &TestSuite::infos,   n_infos,   gts::dictionary<decltype(TestSuite::infos)>>
        >>;

// ----------------------------------------------------------------------------
template <class Meta, class T = typename Meta::target>
void run_test(const char* message, T&& input)
{
    std::cout
        << "//---------------------------------------------------------------\r\n"
        << "// " << message << "\r\n"
        << "//---------------------------------------------------------------\r\n";

    boost::property_tree::ptree tree;
    gts::ptree<Meta>::serialize("Document", input, tree);

    std::stringstream stream;
    boost::property_tree::write_xml(stream, tree, boost::property_tree::xml_writer_settings<std::string>{'\t', 1});
    std::cout << "BUFFER: \r\n" << stream.str() << "\r\n";

    T output;
    gts::ptree_error error;
    //gts::ptree<Meta>::deserialize(tree, "Document", output, &error);
    //std::cout << "INPUT: \r\n" << input << "\r\n\r\n";
    //std::cout << "OUTPUT: \r\n" << output << "\r\n\r\n";
}

// ----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    run_test<TestMeta>("TestSuite", Test{ 32, std::string("obj 32"), true });

    /*
    run_test<TestSuiteMeta>("TestSuite",
        TestSuite{
            { Test{ 32, std::string("obj 32"), true }, Test{ 56, std::string("obj 56"), false } },
            { 1, 2, 3 },
            { { "test one", "just a simple test" },{ "test two", "more complex test" } } });
    */
	return 0;
}




