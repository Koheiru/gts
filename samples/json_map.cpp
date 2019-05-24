#include <sstream>
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
#include <gts/json.h>

// ----------------------------------------------------------------------------
template <class container_t>
std::string format_values(const container_t& values)
{
    if (values.empty())
        return std::string("{}");

    std::stringstream stream;
    stream << "{ ";

    for (const auto& value : values)
        stream << "'" << value.first << "': '" << value.second << "' ,";

    stream.seekp(-1, std::ios_base::end);
    stream << "}";

    return stream.str();
}

// ----------------------------------------------------------------------------
std::string format_ptree(const boost::property_tree::ptree& tree)
{
    std::stringstream stream;
    boost::property_tree::write_json(stream, tree, true);
    return stream.str();
}

// ----------------------------------------------------------------------------
template <class container_t>
boost::property_tree::ptree serialize_by_ptree(const container_t& values)
{
    boost::property_tree::ptree tree;
    for (const auto& value : values)
        tree.put(value.first, value.second);
    return tree;
}

// ----------------------------------------------------------------------------
template <class container_t>
boost::property_tree::ptree serialize_by_gts(const container_t& values)
{
    return boost::property_tree::ptree{};
}

// ----------------------------------------------------------------------------
template <class container_t>
void run_sample(const std::string& name, const container_t& values)
{
    std::cout << " ------------ " << name << " ------------ " << std::endl;
    std::cout << "Values: " << format_values(values) << std::endl;
    std::cout << "Serialization (ptree/boost): " << format_ptree(serialize_by_ptree(values)) << std::endl;
    std::cout << "Serialization (gts/boost):   " << format_ptree(serialize_by_gts(values)) << std::endl;
}

// ----------------------------------------------------------------------------
int main()
{
    run_sample("vector<bool>", std::vector<bool>{ true, true, false, false, true });
    run_sample("vector<uint32_t>", std::vector<std::uint32_t>{ 456, 789123, 115654, 11 });
    run_sample("vector<double>", std::vector<double>{ -0.01, 0.54, 5.65, 9.894, -45.0 });
    run_sample("vector<string>", std::vector<std::string>{ "one", "two", "three", "four" });
    return 0;
}












