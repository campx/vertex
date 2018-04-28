#include <gtest/gtest.h>
#include <vertex/Node.h>
#include <vertex/PathMap.h>

using TestNode = vertex::Node<std::string, std::string>;
using Container = std::map<std::string, TestNode>;
using LinkArray = std::vector<std::string>;
using NodeArray = std::vector<std::pair<const std::string, TestNode>>;
using PathMap = vertex::PathMap<Container>;

TEST(vertex, PathMap)
{ // Create a graph of paths
    auto root = std::make_pair("/", TestNode("Root", LinkArray{"home"}));
    auto home = std::make_pair("home", TestNode("", LinkArray{"jim", "bob"}));
    auto jim = std::make_pair("jim", TestNode("Jim Morris"));
    auto bob = std::make_pair("bob", TestNode("Bob", LinkArray{"documents"}));
    auto documents = std::make_pair("documents", TestNode("Docs"));
    auto vertices = Container{root, home, jim, bob, documents};
    auto search_path = LinkArray{"/", "home", "bob", "documents"};
    auto root_it = vertices.find("/");
    ASSERT_NE(vertices.end(), root_it);
    auto path_map = PathMap(vertices, root_it);
    auto expected = NodeArray{home, bob, documents};
    auto result = path_map.find(search_path);
    EXPECT_EQ(search_path, (*result).first);
}
