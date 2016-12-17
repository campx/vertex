#include "TestNode.h"
#include "gtest/gtest.h"

namespace merkle
{

TestNode::TestNode(std::string data)
    : Node<std::string, std::size_t, TestNode>(data)
{
}

TestNode::value_type TestNode::computeHash() const
{
    auto input = data();
    for (const auto& child : children())
    {
        input.append(std::to_string(child));
    }
    return std::hash<std::string>()(input);
}

TestNode::~TestNode() {}

TEST(Merkle, Node)
{
    // Initialise node
    auto node = TestNode();

    // Add data and hash
    auto data = std::string("hello");
    auto hash = TestNode::value_type(2762169579135187400);
    node.data(data);
    EXPECT_EQ(data, node.data());
    EXPECT_EQ(hash, node.hash());
    EXPECT_TRUE(node.empty());

    // Add a child and rehash
    auto child = TestNode("world");
    auto child_hash = TestNode::value_type(8751027807033337960);
    EXPECT_EQ(child_hash, child.hash());
    auto child_it = node.insert(node.end(), child.hash());
    EXPECT_NE(hash, node.hash());
    EXPECT_FALSE(node.empty());
    EXPECT_EQ(TestNode::size_type(1), node.size());
    EXPECT_EQ(std::hash<std::string>()("hello8751027807033337960"),
              node.hash());

    // Try re-inserting child
    auto size_before = node.size();
    node.insert(child.hash());
    auto size_after = node.size();
    EXPECT_EQ(size_before, size_after);

    // Remove child
    auto child_found = node.find(child.hash());
    EXPECT_EQ(child_it, child_found);
    node.erase(node.find(child.hash()));
    EXPECT_TRUE(node.empty());
    EXPECT_EQ(hash, node.hash()); // should be back to original hash
}

} // namespace merkle
