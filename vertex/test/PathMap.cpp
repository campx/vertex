#include <gtest/gtest.h>
#include <vertex/Node.h>
#include <vertex/PathMap.h>

using TestNode = vertex::Node<std::string, std::string>;
using Container = std::map<std::string, TestNode>;
using LinkArray = std::vector<std::string>;
using NodeArray = std::vector<std::pair<const std::string, TestNode>>;
using PathArray = std::vector<std::pair<LinkArray, TestNode>>;
using PathMap = vertex::PathMap<Container>;

namespace
{

std::ostream& operator<<(std::ostream& output, const TestNode& node)
{
    output << node.data();
    return output;
}
}

TEST(vertex, PathMap)
{ // Create a graph of paths
    auto root = std::make_pair("/", TestNode("Root", LinkArray{"home"}));
    auto home = std::make_pair("home", TestNode("", LinkArray{"jim", "bob"}));
    auto jim = std::make_pair("jim", TestNode("Jim Morris"));
    auto bob_links = LinkArray{"documents", "photos"};
    auto bob = std::make_pair("bob", TestNode("Bob", bob_links));
    auto documents = std::make_pair("documents", TestNode("Docs"));
    auto photos = std::make_pair("photos", TestNode("Pictures"));
    auto expected_vertices = Container{root, home, jim, bob, documents, photos};
    auto vertices = expected_vertices;
    auto root_it = vertices.find("/");
    ASSERT_NE(vertices.end(), root_it);
    auto path_map = PathMap(vertices, root_it);
    auto search_path = LinkArray{"home", "bob", "documents"};

    auto result = path_map.search(search_path);
    ASSERT_NE(result, path_map.end());
    EXPECT_EQ((LinkArray{"home", "bob", "documents"}), result->first);
    result = path_map.find(search_path);
    ASSERT_NE(result, path_map.end());
    EXPECT_EQ(search_path, (*result).first);
    search_path.emplace_back("plans.doc");
    EXPECT_EQ(path_map.end(), path_map.find(search_path));
    EXPECT_NE(result, path_map.end());
    EXPECT_EQ(expected_vertices, vertices);
    auto var = std::make_pair("var", TestNode("", LinkArray{"log"}));
    auto log = std::make_pair("log", TestNode("", LinkArray{"messages"}));
    auto messages = std::make_pair("messages", TestNode());
    expected_vertices.insert(var);
    expected_vertices.insert(log);
    expected_vertices.insert(messages);
    auto path = LinkArray{"var", "log", "messages"};
    path_map.insert(std::make_pair(path, TestNode("Log messages")));
    EXPECT_NE(path_map.search(path), path_map.end());

    /** Check full content of map is as expected
     *
    0 /
    |--------------------
    |                   |
    1 home                var
    |-----              |
    |    |              |
    jim  bob            log
    |-----------        |
    |          |        |
    documents  photos   messages

    find output is depth first, e.g.:
    /home
    /home/bob
    /home/jim
    /home/jim/documents
    /home/jim/photos
    /var
    /var/log
    /var/log/messages
    */
    auto expected_paths = PathArray{
        std::make_pair(LinkArray{"home"}, home.second),
        std::make_pair(LinkArray{"home", "jim"}, jim.second),
        std::make_pair(LinkArray{"home", "bob"}, bob.second),
        std::make_pair(LinkArray{"home", "bob", "documents"}, documents.second),
        std::make_pair(LinkArray{"home", "bob", "photos"}, photos.second),
        std::make_pair(LinkArray{"var"}, var.second),
        std::make_pair(LinkArray{"var", "log"}, log.second),
        std::make_pair(LinkArray{"var", "log", "messages"}, messages.second)};
    auto actual_paths = PathArray{};
    std::copy(path_map.begin(), path_map.end(),
              std::back_inserter(actual_paths));
    EXPECT_EQ(expected_paths, actual_paths);
    EXPECT_NE(vertices["/"], root.second);
    EXPECT_NE(expected_vertices, vertices);
    expected_vertices["/"] = TestNode("Root", LinkArray{"home", "var"});
    EXPECT_EQ(expected_vertices, vertices);

}
