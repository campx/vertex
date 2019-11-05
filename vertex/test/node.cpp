#include "node.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace test {

std::ostream& operator<<(std::ostream& output, const TestNode& node) {
  output << *node;
  return output;
}

TEST(vertex, Node) {
  // Initialise node
  {
    auto node = TestNode();
    EXPECT_TRUE(node.empty());
  }

  // Add data and edge
  {
    auto data = std::string("hello");
    TestNode node;
    *node = data;
    auto child = TestNode::link_type(12345678);
    EXPECT_EQ(data, *node);
    EXPECT_EQ(node.end(), node.find(child));
    node.insert(child);
    EXPECT_NE(node.end(), node.find(child));
    EXPECT_FALSE(node.empty());
    EXPECT_EQ(std::size_t(1), node.size());
    auto erased = node.erase(child);
    EXPECT_THAT(erased, ::testing::Gt(0u));
    EXPECT_TRUE(node.empty());
  }

  // Compare vertices
  {
    TestNode node;
    *node = "world";

    std::vector<TestNode::link_type> children;
    children.emplace_back(2762169579135187400u);
    children.emplace_back(8751027807033337960u);
    node.insert(children.begin(), children.end());
    EXPECT_FALSE(node.empty());
    EXPECT_EQ(std::size_t(2), node.size());
    EXPECT_TRUE(
        std::equal(node.begin(), node.end(), children.begin(), children.end()));
    auto other = node;
    EXPECT_EQ(other, node);
    other.insert(TestLink(898989));
    EXPECT_NE(other, node);
    auto other_copy = other;
    auto node_copy = node;
    std::swap(other, node);
    EXPECT_EQ(node_copy, other);
    EXPECT_EQ(other_copy, node);
    node.clear();
    EXPECT_TRUE(node.empty());
  }
}

}  // namespace test
