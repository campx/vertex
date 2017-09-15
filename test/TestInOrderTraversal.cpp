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

struct Tree : public ::testing::Test
{
    VertexMap vertices;
    Tree()
    {
        /*******************\
         *         F        *
         *        / \       *
         *       /   \      *
         *      B     G     *
         *     / \     \    *
         *    A   D     I   *
         *       / \   /    *
         *      C   E H     *
        \*******************/

        vertices.insert(std::make_pair("A", TestVertex("A")));
        vertices.insert(std::make_pair("C", TestVertex("C")));
        vertices.insert(std::make_pair("E", TestVertex("E")));
        vertices.insert(std::make_pair("H", TestVertex("H")));
        auto d = TestVertex("D");
        d.insert("C");
        d.insert("E");
        vertices.insert(std::make_pair("D", d));
        auto b = TestVertex("B");
        b.insert("A");
        b.insert("D");
        vertices.insert(std::make_pair("B", b));
        auto f = TestVertex("F");
        f.insert("B");
        f.insert("G");
        vertices.insert(std::make_pair("F", f));
        auto g = TestVertex("G");
        g.insert("");
        g.insert("I");
        vertices.insert(std::make_pair("G", g));
        auto i = TestVertex("I");
        i.insert("H");
        i.insert("");
        vertices.insert(std::make_pair("I", i));
    }
};

TEST_F(Tree, InOrderTraversal)
{
    using Iot = InOrderTraversal<VertexMap>;
    {
        auto vertex_it = VertexIterator<Iot>(&vertices, vertices.find("F"));
        auto vertex_order = std::ostringstream{};
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("ABCDEFGHI", vertex_order.str());
    }
    { // traverse sub tree rooted at B
        auto vertex_it = VertexIterator<Iot>(&vertices, vertices.find("B"));
        auto vertex_order = std::ostringstream{};
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("ABCDE", vertex_order.str());
    }
    { // traverse sub tree rooted at G
        auto vertex_it = VertexIterator<Iot>(&vertices, vertices.find("G"));
        auto vertex_order = std::ostringstream{};
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("GHI", vertex_order.str());
    }
}

} // namespace vertex
