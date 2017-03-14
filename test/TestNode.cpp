#include "TestNode.h"
#include "gtest/gtest.h"

namespace merkle
{

std::ostream& operator<<(std::ostream& output, const TestNode& node)
{
    output << node.data();
    return output;
}

TestNode::value_type TestNode::self_link() const
{
    auto input = data();
    for (const auto& link : links())
    {
        input.append(std::to_string(link));
    }
    return std::hash<std::string>()(input);
}

TEST(Merkle, Node)
{
    // Initialise node
    {
        auto node = TestNode();
        auto hash = TestNode::value_type(6142509188972423790);
        EXPECT_EQ(hash, node.hash());
    }

    // Add data and hash
    {
        auto data = std::string("hello");
        auto node = TestNode(data);
        auto hash = TestNode::value_type(2762169579135187400);
        EXPECT_EQ(data, node.data());
        EXPECT_EQ(hash, node.hash());
        EXPECT_TRUE(node.links().empty());
    }

    // Add a child and rehash
    {
        auto child = TestNode("world");
        auto child_hash = TestNode::value_type(8751027807033337960);
        EXPECT_EQ(child_hash, child.hash());

        auto hash = TestNode::value_type(2762169579135187400);
        std::vector<TestNode::value_type> children;
        children.push_back(child.hash());
        auto node = TestNode("hello", children);
        EXPECT_NE(hash, node.hash());
        EXPECT_FALSE(node.links().empty());
        EXPECT_EQ(std::size_t(1), node.links().size());
        EXPECT_EQ(std::hash<std::string>()("hello8751027807033337960"),
                  node.hash());
    }
}

} // namespace merkle
