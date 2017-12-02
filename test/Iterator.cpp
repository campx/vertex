#include "gtest/gtest.h"
#include <vertex/BreadthFirstTraversal.h>
#include <vertex/EdgeIterator.h>
#include <vertex/Node.h>
#include <vertex/NodeIterator.h>

namespace
{

using TestNode = vertex::Node<std::string, std::string>;
using Container = std::map<std::string, TestNode>;

std::ostream& operator<<(std::ostream& output, const TestNode& vertex)
{
    output << vertex.data();
    return output;
}

} // namespace

namespace vertex
{

TEST(Node, NodeIterator)
{

    /************************
     *          A           *
     *         / \          *
     *        B   C         *
     *       /\   /\        *
     *      D  E F  G       *
     ***********************/
    auto vertices = Container{};
    vertices.insert(std::make_pair("G", TestNode("G")));
    vertices.insert(std::make_pair("F", TestNode("F")));
    vertices.insert(std::make_pair("E", TestNode("E")));
    vertices.insert(std::make_pair("D", TestNode("D")));
    {
        auto vertex = TestNode("C");
        vertex.insert(vertex.end(), "F");
        vertex.insert(vertex.end(), "G");
        vertices.insert(std::make_pair("C", vertex));
    }
    {
        auto vertex = TestNode("B");
        vertex.insert(vertex.end(), "D");
        vertex.insert(vertex.end(), "E");
        vertices.insert(std::make_pair("B", vertex));
    }
    {
        auto vertex = TestNode("A");
        vertex.insert(vertex.end(), "B");
        vertex.insert(vertex.end(), "C");
        vertices.insert(std::make_pair("A", vertex));
    }
    EXPECT_FALSE(vertices.empty());
    {
        using Bfs = BreadthFirstTraversal<Container>;
        auto it = NodeIterator<Bfs>(&vertices, vertices.begin());
        EXPECT_NE(begin(it), end(it));
        auto os = std::ostringstream();
        for (const auto& vertex : it)
        {
            os << vertex;
        }
        EXPECT_EQ("ABCDEFG", os.str());
    }

    vertices.clear();
    { // traverse empty graph

        using Bfs = BreadthFirstTraversal<Container>;
        auto it = NodeIterator<Bfs>(&vertices, vertices.begin());
        EXPECT_EQ(begin(it), end(it));
        auto os = std::ostringstream();
        for (const auto& vertex : it)
        {
            os << vertex;
        }
        EXPECT_EQ("", os.str());
    }
}

} // namespace vertex
