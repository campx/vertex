#include "Node.h"
#include "gtest/gtest.h"

namespace vertex
{

std::ostream& operator<<(std::ostream& output, const TestNode& node)
{
    output << node.data();
    return output;
}

TEST(vertex, Node)
{
    // Initialise node
    {
        auto node = TestNode();
        EXPECT_TRUE(node.links().empty());
    }

    // Add data and edge
    {
        auto data = std::string("hello");
        auto node = TestNode(data);
        auto child = TestNode::link_type(12345678);
        EXPECT_EQ(data, node.data());
        EXPECT_EQ(node.links().end(),
                  std::find(node.links().begin(), node.links().end(), child));
        node.links().insert(node.links().end(), child);
        EXPECT_NE(node.links().end(),
                  std::find(node.links().begin(), node.links().end(), child));
        EXPECT_FALSE(node.links().empty());
        EXPECT_EQ(std::size_t(1), node.links().size());
        auto erased = node.links().erase(
            std::find(node.links().begin(), node.links().end(), child));
        EXPECT_EQ(node.links().end(), erased);
        EXPECT_TRUE(node.links().empty());
    }

    // Compare vertices
    {
        auto node = TestNode("world");

        std::vector<TestNode::link_type> children;
        children.push_back(
            TestNode::link_type(std::size_t(2762169579135187400)));
        children.push_back(
            TestNode::link_type(std::size_t(8751027807033337960)));
        node.links().insert<std::vector<TestNode::link_type>::iterator>(
            node.links().end(), children.begin(), children.end());
        EXPECT_FALSE(node.links().empty());
        EXPECT_EQ(std::size_t(2), node.links().size());
        EXPECT_TRUE(std::equal(node.links().begin(), node.links().end(),
                               children.begin(), children.end()));
        auto other = node;
        EXPECT_EQ(other, node);
        other.links().insert(other.links().end(), TestLink(898989));
        EXPECT_NE(other, node);
        auto other_copy = other;
        auto node_copy = node;
        std::swap(other, node);
        EXPECT_EQ(node_copy, other);
        EXPECT_EQ(other_copy, node);
        node.links().clear();
        EXPECT_TRUE(node.links().empty());
    }
}

} // namespace vertex
