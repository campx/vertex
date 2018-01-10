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
        EXPECT_TRUE(node.empty());
    }

    // Add data and edge
    {
        auto data = std::string("hello");
        auto node = TestNode(data);
        auto child = TestNode::link_type(12345678);
        EXPECT_EQ(data, node.data());
        EXPECT_EQ(node.end(), node.find(child));
        node.insert(node.end(), child);
        EXPECT_NE(node.end(), node.find(child));
        EXPECT_FALSE(node.empty());
        EXPECT_EQ(std::size_t(1), node.size());
        auto erased = node.erase(node.find(child));
        EXPECT_EQ(node.end(), erased);
        EXPECT_TRUE(node.empty());
    }

    // Compare vertices
    {
        auto node = TestNode("world");

        std::vector<TestNode::link_type> children;
        children.push_back(TestNode::link_type(2762169579135187400));
        children.push_back(TestNode::link_type(8751027807033337960));
        node.insert<std::vector<TestNode::link_type>::iterator>(
            node.end(), children.begin(), children.end());
        EXPECT_FALSE(node.empty());
        EXPECT_EQ(std::size_t(2), node.size());
        EXPECT_TRUE(std::equal(node.begin(), node.end(), children.begin(),
                               children.end()));
        auto other = node;
        EXPECT_EQ(other, node);
        other.insert(other.end(), TestLink(898989));
        EXPECT_NE(other, node);
        auto other_copy = other;
        auto node_copy = node;
        std::swap(other, node);
        EXPECT_EQ(node_copy, other);
        EXPECT_EQ(other_copy, node);
        node.clear();
        EXPECT_TRUE(node.empty());
    }

    // Inserting duplicate children
    {
        auto node = TestNode("!");
        auto const child_a = node.insert(node.end(), TestLink(1));
        auto child_b = node.insert(node.end(), TestLink(1));
        EXPECT_TRUE(child_a.second);
        EXPECT_FALSE(child_b.second);
        EXPECT_EQ(child_a.first, child_b.first); // same unique child
        EXPECT_NE(node.end(), child_a.first);
    }
}

} // namespace vertex
