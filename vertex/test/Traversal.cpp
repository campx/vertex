#include "gtest/gtest.h"
#include <vertex/BreadthFirstTraversal.h>
#include <vertex/InOrderTraversal.h>
#include <vertex/Node.h>
#include <vertex/PostOrderTraversal.h>
#include <vertex/PreOrderTraversal.h>

namespace
{

using TestNode = vertex::Node<std::string, std::string>;
using Container = std::map<std::string, TestNode>;
using LinkArray = typename TestNode::container_type;

std::ostream& operator<<(std::ostream& output, const TestNode& vertex)
{
    output << vertex.data();
    return output;
}

} // namespace

namespace vertex
{
struct Graph : public ::testing::Test
{
    Container vertices;

    Graph()
    {
        /*******************\
         *         1        *
         *        /|\       *
         *       / | \      *
         *      2  7  8     *
         *     / \   / \    *
         *    3   6 9   12  *
         *   / \   / \      *
         *  4   5 10  11    *
        \*******************/

        /** Create leaf nodes */
        vertices.insert(
            std::make_pair("1", TestNode("1", LinkArray{"2", "7", "8"})));
        vertices.insert(
            std::make_pair("2", TestNode("2", LinkArray{"3", "6"})));
        vertices.insert(
            std::make_pair("3", TestNode("3", LinkArray{"4", "5"})));
        vertices.insert(std::make_pair("4", TestNode("4")));
        vertices.insert(std::make_pair("5", TestNode("5")));
        vertices.insert(std::make_pair("6", TestNode("6")));
        vertices.insert(std::make_pair("7", TestNode("7")));
        vertices.insert(
            std::make_pair("8", TestNode("8", LinkArray{"9", "12"})));
        vertices.insert(
            std::make_pair("9", TestNode("9", LinkArray{"10", "11"})));
        vertices.insert(std::make_pair("10", TestNode("10")));
        vertices.insert(std::make_pair("11", TestNode("11")));
        vertices.insert(std::make_pair("12", TestNode("12")));
    }
};

TEST_F(Graph, PreOrderTraversal)
{
    using Pot = PreOrderTraversal<Container>;
    {
        auto traversal = Pot(vertices, vertices.find("1"));
        auto vertex_order = std::ostringstream();
        for (const auto& v : traversal)
        {
            vertex_order << v.second;
        }
        EXPECT_EQ("123456789101112", vertex_order.str());
    }
}

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
        d.links().insert(d.links().end(), "C");
        d.links().insert(d.links().end(), "E");
        vertices.insert(std::make_pair("D", d));
        auto b = TestNode("B");
        b.links().insert(b.links().end(), "A");
        b.links().insert(b.links().end(), "D");
        vertices.insert(std::make_pair("B", b));
        auto f = TestNode("F");
        f.links().insert(f.links().end(), "B");
        f.links().insert(f.links().end(), "G");
        vertices.insert(std::make_pair("F", f));
        auto g = TestNode("G");
        g.links().insert(g.links().end(), "");
        g.links().insert(g.links().end(), "I");
        vertices.insert(std::make_pair("G", g));
        auto i = TestNode("I");
        i.links().insert(i.links().end(), "H");
        i.links().insert(i.links().end(), "");
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
        output << i.second.data();
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
        auto traversal = Pot(vertices, vertices.find("F"));
        auto vertex_order = std::ostringstream();
        for (const auto& v : traversal)
        {
            vertex_order << v.second;
        }
        EXPECT_EQ("FBADCEGIH", vertex_order.str());
    }
}

TEST_F(Tree, InOrderTraversal)
{
    using Iot = InOrderTraversal<Container>;
    {
        auto traversal = Iot(vertices, vertices.find("F"));
        auto vertex_order = std::ostringstream();
        for (const auto& v : traversal)
        {
            vertex_order << v.second;
        }
        EXPECT_EQ("ABCDEFGHI", vertex_order.str());
    }
    { // traverse sub tree rooted at B
        auto traversal = Iot(vertices, vertices.find("B"));
        auto vertex_order = std::ostringstream();
        for (const auto& v : traversal)
        {
            vertex_order << v.second;
        }
        EXPECT_EQ("ABCDE", vertex_order.str());
    }
    { // traverse sub tree rooted at G
        auto traversal = Iot(vertices, vertices.find("G"));
        auto vertex_order = std::ostringstream();
        for (const auto& v : traversal)
        {
            vertex_order << v.second;
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
        auto traversal = Iot(vertices, vertices.find("F"), predicate);
        auto vertex_order = std::ostringstream();
        for (const auto& v : traversal)
        {
            vertex_order << v.second;
        }
        EXPECT_EQ("FGI", vertex_order.str());
    }
}

TEST_F(Tree, PredicatedBreadthFirstTraversal)
{
    using Predicate = std::function<bool(
        const std::pair<Container::key_type, Container::key_type>&)>;
    auto predicate = Predicate(
        [](const auto& e) -> auto { return e.first == "F"; });

    using Bft = BreadthFirstTraversal<Container, Predicate>;
    {
        auto traversal = Bft(vertices, vertices.find("F"), predicate);
        auto vertex_order = std::ostringstream();
        for (const auto& v : traversal)
        {
            vertex_order << v.second;
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
        auto traversal = Bft(vertices, vertices.find("F"), predicate);
        auto vertex_order = std::ostringstream();
        for (const auto& v : traversal)
        {
            vertex_order << v.second;
        }
        EXPECT_EQ("FBGADI", vertex_order.str());
    }
}

TEST_F(Tree, PostOrderTraversal)
{
    using Pot = PostOrderTraversal<Container>;
    auto traversal = Pot(vertices, vertices.find("F"));
    auto vertex_order = std::ostringstream();
    auto i = 0;
    for (const auto& v : traversal)
    {
        vertex_order << v.second;
        if (++i > 10)
        {
            break;
        }
    }
    EXPECT_EQ("ACEDBHIGF", vertex_order.str());
}

TEST_F(Tree, BredthFirstTraversal)
{
    vertices.clear();
    EXPECT_TRUE(vertices.empty());
    vertices.insert(std::make_pair("B", TestNode("B")));
    vertices.insert(std::make_pair("C", TestNode("C")));
    vertices.insert(std::make_pair("D", TestNode("D")));
    auto root = TestNode("A");
    root.links().insert(root.links().end(), "B");
    root.links().insert(root.links().end(), "C");
    root.links().insert(root.links().end(), "D");
    vertices.insert(std::make_pair("A", root));
    { // traverse graph of depth 1
        using Bfs = BreadthFirstTraversal<Container>;
        auto traversal = Bfs(vertices, vertices.find("A"));
        auto os = std::ostringstream();
        for (const auto& vertex : traversal)
        {
            os << vertex.second;
        }
        EXPECT_EQ("ABCD", os.str());
    }
}

TEST_F(Tree, EmptyTraversal)
{
    vertices.clear();
    { // traverse graph of depth 1
        using Bfs = BreadthFirstTraversal<Container>;
        EXPECT_EQ(vertices.end(), vertices.find("A"));
        auto traversal = Bfs(vertices, vertices.end());
        EXPECT_EQ(traversal.end(), traversal);
        auto os = std::ostringstream();
        for (const auto& vertex : traversal)
        {
            os << vertex.second;
        }
        EXPECT_EQ("", os.str());
    }
}

} // namespace vertex
