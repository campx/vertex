#include "gtest/gtest.h"
#include <vertex/BreadthFirstTraversal.h>
#include <vertex/EdgeIterator.h>
#include <vertex/Vertex.h>
#include <vertex/VertexIterator.h>

namespace
{

using TestVertex = vertex::Vertex<std::string, std::string>;
using Container = std::map<std::string, TestVertex>;

std::ostream& operator<<(std::ostream& output, const TestVertex& vertex)
{
    output << vertex.data();
    return output;
}

} // namespace

namespace vertex
{

TEST(Vertex, VertexIterator)
{

    /************************
     *          A           *
     *         / \          *
     *        B   C         *
     *       /\   /\        *
     *      D  E F  G       *
     ***********************/
    auto vertices = Container{};
    vertices.insert(std::make_pair("G", TestVertex("G")));
    vertices.insert(std::make_pair("F", TestVertex("F")));
    vertices.insert(std::make_pair("E", TestVertex("E")));
    vertices.insert(std::make_pair("D", TestVertex("D")));
    {
        auto vertex = TestVertex("C");
        vertex.insert(vertex.end(), "F");
        vertex.insert(vertex.end(), "G");
        vertices.insert(std::make_pair("C", vertex));
    }
    {
        auto vertex = TestVertex("B");
        vertex.insert(vertex.end(), "D");
        vertex.insert(vertex.end(), "E");
        vertices.insert(std::make_pair("B", vertex));
    }
    {
        auto vertex = TestVertex("A");
        vertex.insert(vertex.end(), "B");
        vertex.insert(vertex.end(), "C");
        vertices.insert(std::make_pair("A", vertex));
    }
    EXPECT_FALSE(vertices.empty());
    {
        using Bfs = BreadthFirstTraversal<Container>;
        auto it = VertexIterator<Bfs>(&vertices, vertices.begin());
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
        auto it = VertexIterator<Bfs>(&vertices, vertices.begin());
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
