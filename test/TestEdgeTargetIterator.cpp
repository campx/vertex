#include "gtest/gtest.h"
#include <vertex/BfsEdgeIterator.h>
#include <vertex/EdgeTargetIterator.h>
#include <vertex/Vertex.h>

namespace vertex
{

using TestVertex = Vertex<std::string, std::string>;
using VertexMap = std::map<std::string, TestVertex>;

std::ostream& operator<<(std::ostream& output, const TestVertex& vertex)
{
    output << vertex.data();
    return output;
}

TEST(Vertex, EdgeTargetIterator)
{

    /************************
     *          A           *
     *         / \          *
     *        B   C         *
     *       /\   /\        *
     *      D  E F  G       *
     ***********************/
    auto vertices = VertexMap{};
    vertices.insert(std::make_pair("G", TestVertex("G")));
    vertices.insert(std::make_pair("F", TestVertex("F")));
    vertices.insert(std::make_pair("E", TestVertex("E")));
    vertices.insert(std::make_pair("D", TestVertex("D")));
    {
        auto vertex = TestVertex("C");
        vertex.insert("F");
        vertex.insert("G");
        vertices.insert(std::make_pair("C", vertex));
    }
    {
        auto vertex = TestVertex("B");
        vertex.insert("D");
        vertex.insert("E");
        vertices.insert(std::make_pair("B", vertex));
    }
    {
        auto vertex = TestVertex("A");
        vertex.insert("B");
        vertex.insert("C");
        vertices.insert(std::make_pair("A", vertex));
    }
    EXPECT_FALSE(vertices.empty());
    {
        auto bfs = BfsEdgeIterator<VertexMap>(&vertices, vertices.begin());
        auto it = EdgeTargetIterator<BfsEdgeIterator<VertexMap>>(bfs);
        EXPECT_NE(begin(it), end(it));
        auto os = std::ostringstream{};
        for (const auto& vertex : it)
        {
            os << vertex;
        }
        EXPECT_EQ("ABCDEFG", os.str());
    }

    vertices.clear();
    { // traverse empty graph
        auto bfs = BfsEdgeIterator<VertexMap>(&vertices, vertices.begin());
        auto it = EdgeTargetIterator<BfsEdgeIterator<VertexMap>>(bfs);
        EXPECT_EQ(begin(it), end(it));
        auto os = std::ostringstream{};
        for (const auto& vertex : it)
        {
            os << vertex;
        }
        EXPECT_EQ("", os.str());
    }
}

} // namespace vertex
