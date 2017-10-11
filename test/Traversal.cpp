#include "gtest/gtest.h"
#include <vertex/BredthFirstTraversal.h>
#include <vertex/EdgeIterator.h>
#include <vertex/InOrderTraversal.h>
#include <vertex/PostOrderTraversal.h>
#include <vertex/PreOrderTraversal.h>
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

std::ostream& operator<<(std::ostream& output,
                         const std::pair<std::string, std::string>& edge)
{
    output << edge.first << edge.second;
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

TEST_F(Tree, InOrderIterator)
{
    auto traversal =
        InOrderTraversal<VertexMap>(&vertices, vertices.find("F"));
    auto output = std::ostringstream{};
    auto count = 0;
    for (const auto& i : traversal)
    {
        output << i.vertex().data();
        if (++count > 10)
        {
            break;
        }
    }
    EXPECT_EQ("ABCDEFGHI", output.str());
}

TEST_F(Tree, PreOrderTraversal)
{
    using Pot = PreOrderTraversal<VertexMap>;
    {
        auto vertex_it = VertexIterator<Pot>(&vertices, vertices.find("F"));
        auto vertex_order = std::ostringstream{};
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("FBADCEGIH", vertex_order.str());
    }
}

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

TEST_F(Tree, PredicatedInOrderTraversal)
{
    using Predicate = std::function<bool(
        const std::pair<VertexMap::key_type, VertexMap::key_type>&)>;
    auto predicate = Predicate([](const auto& e) -> auto {
        return e.second == "G" || e.second == "F" || e.second == "I";
    });

    using Iot = InOrderTraversal<VertexMap, Predicate>;
    {
        auto vertex_it =
            VertexIterator<Iot>(&vertices, vertices.find("F"), predicate);
        auto vertex_order = std::ostringstream{};
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("FGI", vertex_order.str());
    }
}

TEST_F(Tree, PostOrderTraversal)
{
    using Pot = PostOrderTraversal<VertexMap>;
    auto vertex_it = VertexIterator<Pot>(&vertices, vertices.find("F"));
    auto vertex_order = std::ostringstream{};
    auto i = 0;
    for (const auto& v : vertex_it)
    {
        vertex_order << v;
        if (++i > 10)
        {
            break;
        }
    }
    EXPECT_EQ("ACEDBHIGF", vertex_order.str());
}

TEST_F(Tree, BredthFirstTraversal)
{
    EXPECT_FALSE(vertices.empty());
    { // extract edges and vertices during a single traversal of the tree
        using Bfs = BredthFirstTraversal<VertexMap>;
        auto edge_it = EdgeIterator<Bfs>(&vertices, vertices.find("F"));
        auto vertex_it = VertexIterator<Bfs>(edge_it.traversal());
        EXPECT_EQ(edge_it.traversal(), vertex_it.traversal());
        EXPECT_NE(begin(edge_it), end(edge_it));
        auto edge_csv = std::ostringstream{};
        auto vertex_csv = std::ostringstream{};
        for (auto end_it = std::end(edge_it); edge_it != end_it; ++edge_it)
        {
            edge_csv << *edge_it << ", ";
            vertex_csv << *vertex_it;
        }
        EXPECT_EQ("F, FB, FG, BA, BD, GI, DC, DE, IH, ", edge_csv.str());
        EXPECT_EQ("FBGADICEH", vertex_csv.str());
    }
    vertices.clear();
    { // traverse empty graph
        using Bfs = BredthFirstTraversal<VertexMap>;
        auto traversal = std::make_shared<Bfs>(&vertices, vertices.begin());

        // Create an accessor which pulls out edges from a traversal
        using EdgeAccess = std::function<const Bfs::edge_type&(const Bfs&)>;
        auto accessor =
            EdgeAccess([](const auto& t) -> auto { return t.edge(); });

        auto it = make_iterator(traversal, accessor);
        EXPECT_EQ(begin(it), end(it));
        auto os = std::ostringstream{};
        for (const auto& edge : it)
        {
            os << edge << ", ";
        }
        EXPECT_EQ("", os.str());
    }
    vertices.insert(std::make_pair("B", TestVertex("B")));
    vertices.insert(std::make_pair("C", TestVertex("C")));
    vertices.insert(std::make_pair("D", TestVertex("D")));
    auto root = TestVertex("A");
    root.insert("B");
    root.insert("C");
    root.insert("D");
    vertices.insert(std::make_pair("A", root));
    { // traverse graph of depth 1
        using Bfs = BredthFirstTraversal<VertexMap>;
        auto it = VertexIterator<Bfs>(&vertices, vertices.find("A"));
        auto os = std::ostringstream{};
        for (const auto& vertex : it)
        {
            os << vertex;
        }
        EXPECT_EQ("ABCD", os.str());
    }
}

} // namespace vertex
