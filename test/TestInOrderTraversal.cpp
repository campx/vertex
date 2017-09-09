#include "gtest/gtest.h"
#include <vertex/EdgeIterator.h>
#include <vertex/InOrderTraversal.h>
#include <vertex/Vertex.h>
#include <vertex/VertexIterator.h>

namespace
{

using TestVertex = vertex::Vertex<std::string, std::string>;
using VertexMap = std::map<std::string, TestVertex>;

std::ostream& operator<<(std::ostream& output, const TestVertex& vertex)
{
    output << vertex.data();
    return output;
}

} // namespace

namespace vertex
{

TEST(Vertex, InOrderTraversal)
{
    /************************
     *          D           *
     *         / \          *
     *        B   F         *
     *       /\   /\        *
     *      A  C E  G       *
     ***********************/

    auto vertices = VertexMap{};
    vertices.insert(std::make_pair("A", TestVertex("A")));
    vertices.insert(std::make_pair("C", TestVertex("C")));
    vertices.insert(std::make_pair("E", TestVertex("E")));
    vertices.insert(std::make_pair("G", TestVertex("G")));
    {
        auto vertex = TestVertex("D");
        vertex.insert("B");
        vertex.insert("F");
        vertices.insert(std::make_pair("D", vertex));
    }
    {
        auto vertex = TestVertex("B");
        vertex.insert("A");
        vertex.insert("C");
        vertices.insert(std::make_pair("B", vertex));
    }
    {
        auto vertex = TestVertex("F");
        vertex.insert("E");
        vertex.insert("G");
        vertices.insert(std::make_pair("F", vertex));
    }
    { // extract edges and vertices during a single traversal of the tree
        using Iot = InOrderTraversal<VertexMap>;
        auto vertex_it = VertexIterator<Iot>(&vertices, vertices.find("D"));
        auto vertex_order = std::ostringstream{};
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("ABCDEFG", vertex_order.str());
    }
}

} // namespace vertex
