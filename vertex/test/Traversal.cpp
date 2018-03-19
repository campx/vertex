#include "gtest/gtest.h"
#include <vertex/BreadthFirstTraversal.h>
#include <vertex/EdgeIterator.h>
#include <vertex/InOrderTraversal.h>
#include <vertex/Node.h>
#include <vertex/PostOrderTraversal.h>
#include <vertex/PreOrderTraversal.h>
#include <vertex/Predicate.h>
#include <vertex/VertexIterator.h>

namespace
{

using TestNode = vertex::Node<std::string, std::string>;
using Container = std::map<std::string, TestNode>;

std::ostream& operator<<(std::ostream& output, const TestNode& vertex)
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
    Container vertices;
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

        vertices.insert(std::make_pair("A", TestNode("A")));
        vertices.insert(std::make_pair("C", TestNode("C")));
        vertices.insert(std::make_pair("E", TestNode("E")));
        vertices.insert(std::make_pair("H", TestNode("H")));
        auto d = TestNode("D");
        d.insert(d.end(), "C");
        d.insert(d.end(), "E");
        vertices.insert(std::make_pair("D", d));
        auto b = TestNode("B");
        b.insert(b.end(), "A");
        b.insert(b.end(), "D");
        vertices.insert(std::make_pair("B", b));
        auto f = TestNode("F");
        f.insert(f.end(), "B");
        f.insert(f.end(), "G");
        vertices.insert(std::make_pair("F", f));
        auto g = TestNode("G");
        g.insert(g.end(), "");
        g.insert(g.end(), "I");
        vertices.insert(std::make_pair("G", g));
        auto i = TestNode("I");
        i.insert(i.end(), "H");
        i.insert(i.end(), "");
        vertices.insert(std::make_pair("I", i));
    }
};

TEST_F(Tree, InOrderIterator)
{
    auto traversal = InOrderTraversal<Container>(vertices, vertices.find("F"));
    auto output = std::ostringstream();
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
    using Pot = PreOrderTraversal<Container>;
    {
        auto vertex_it = VertexIterator<Pot>(vertices, vertices.find("F"));
        auto vertex_order = std::ostringstream();
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("FBADCEGIH", vertex_order.str());
    }
}

TEST_F(Tree, InOrderTraversal)
{
    using Iot = InOrderTraversal<Container>;
    {
        auto vertex_it = VertexIterator<Iot>(vertices, vertices.find("F"));
        auto vertex_order = std::ostringstream();
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("ABCDEFGHI", vertex_order.str());
    }
    { // traverse sub tree rooted at B
        auto vertex_it = VertexIterator<Iot>(vertices, vertices.find("B"));
        auto vertex_order = std::ostringstream();
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("ABCDE", vertex_order.str());
    }
    { // traverse sub tree rooted at G
        auto vertex_it = VertexIterator<Iot>(vertices, vertices.find("G"));
        auto vertex_order = std::ostringstream();
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
        const std::pair<Container::key_type, Container::key_type>&)>;
    auto predicate = Predicate([](const auto& e) -> auto {
        return e.second == "G" || e.second == "F" || e.second == "I";
    });

    using Iot = InOrderTraversal<Container, Predicate>;
    {
        auto vertex_it =
            VertexIterator<Iot>(vertices, vertices.find("F"), predicate);
        auto vertex_order = std::ostringstream();
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("FGI", vertex_order.str());
    }
}

TEST_F(Tree, PredicatedBreadthFirstTraversal)
{
    using Predicate = std::function<bool(
        const std::pair<Container::key_type, Container::key_type>&)>;
    auto predicate =
        Predicate([](const auto& e) -> auto { return e.first == "F"; });

    using Bft = BreadthFirstTraversal<Container, Predicate>;
    {
        auto vertex_it =
            VertexIterator<Bft>(vertices, vertices.find("F"), predicate);
        auto vertex_order = std::ostringstream();
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("FBG", vertex_order.str());
    }
}

TEST_F(Tree, MaxDepthBreadthFirstTraversal)
{
    using Predicate = MaxDepthPredicate<Container>;
    auto predicate = Predicate(2);

    using Bft = BreadthFirstTraversal<Container, Predicate>;
    {
        auto vertex_it =
            VertexIterator<Bft>(vertices, vertices.find("F"), predicate);
        auto vertex_order = std::ostringstream();
        for (const auto& v : vertex_it)
        {
            vertex_order << v;
        }
        EXPECT_EQ("FBGADI", vertex_order.str());
    }
}
TEST_F(Tree, PostOrderTraversal)
{
    using Pot = PostOrderTraversal<Container>;
    auto vertex_it = VertexIterator<Pot>(vertices, vertices.find("F"));
    auto vertex_order = std::ostringstream();
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
        using Bfs = BreadthFirstTraversal<Container>;
        auto edge_it = EdgeIterator<Bfs>(vertices, vertices.find("F"));
        auto vertex_it = VertexIterator<Bfs>(edge_it.traversal());
        EXPECT_EQ(edge_it.traversal(), vertex_it.traversal());
        EXPECT_NE(begin(edge_it), end(edge_it));
        auto edge_csv = std::ostringstream();
        auto vertex_csv = std::ostringstream();
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
        using Bfs = BreadthFirstTraversal<Container>;
        auto traversal = std::make_shared<Bfs>(vertices, vertices.begin());

        // Create an accessor which pulls out edges from a traversal
        using EdgeAccess = std::function<const Bfs::edge_type&(const Bfs&)>;
        auto accessor =
            EdgeAccess([](const auto& t) -> auto { return t.edge(); });

        auto it = make_iterator(traversal, accessor);
        EXPECT_EQ(begin(it), end(it));
        auto os = std::ostringstream();
        for (const auto& edge : it)
        {
            os << edge << ", ";
        }
        EXPECT_EQ("", os.str());
    }
    vertices.insert(std::make_pair("B", TestNode("B")));
    vertices.insert(std::make_pair("C", TestNode("C")));
    vertices.insert(std::make_pair("D", TestNode("D")));
    auto root = TestNode("A");
    root.insert(root.end(), "B");
    root.insert(root.end(), "C");
    root.insert(root.end(), "D");
    vertices.insert(std::make_pair("A", root));
    { // traverse graph of depth 1
        using Bfs = BreadthFirstTraversal<Container>;
        auto it = VertexIterator<Bfs>(vertices, vertices.find("A"));
        auto os = std::ostringstream();
        for (const auto& vertex : it)
        {
            os << vertex;
        }
        EXPECT_EQ("ABCD", os.str());
    }
}

} // namespace vertex
