#include "TestVertex.h"
#include "gtest/gtest.h"

namespace vertex
{

std::ostream& operator<<(std::ostream& output, const TestVertex& vertex)
{
    output << vertex.data();
    return output;
}

TEST(Merkle, Vertex)
{
    // Initialise vertex
    {
        auto vertex = TestVertex();
        EXPECT_TRUE(vertex.empty());
    }

    // Add data and edge
    {
        auto data = std::string("hello");
        auto vertex = TestVertex(data);
        auto child = TestVertex::key_type(12345678);
        EXPECT_EQ(data, vertex.data());
        EXPECT_EQ(vertex.end(), vertex.find(child));
        vertex.insert(child);
        EXPECT_NE(vertex.end(), vertex.find(child));
        EXPECT_FALSE(vertex.empty());
        EXPECT_EQ(vertex.end(), vertex.erase(vertex.find(child)));
        EXPECT_TRUE(vertex.empty());
    }

    // Compare vertices
    {
        auto vertex = TestVertex("world");

        std::vector<TestVertex::key_type> children;
        children.push_back(TestVertex::key_type(2762169579135187400));
        children.push_back(TestVertex::key_type(8751027807033337960));
        vertex.insert<std::vector<TestVertex::key_type>::iterator>(
            vertex.end(), children.begin(), children.end());
        EXPECT_FALSE(vertex.empty());
        EXPECT_EQ(std::size_t(2), vertex.size());
        EXPECT_TRUE(std::equal(vertex.begin(), vertex.end(), children.begin(),
                               children.end()));
        auto other = vertex;
        EXPECT_EQ(other, vertex);
        other.insert(898989);
        EXPECT_NE(other, vertex);
        auto other_copy = other;
        auto vertex_copy = vertex;
        std::swap(other, vertex);
        EXPECT_EQ(vertex_copy, other);
        EXPECT_EQ(other_copy, vertex);
        vertex.clear();
        EXPECT_TRUE(vertex.empty());
    }

    // Inserting duplicate children
    {
        auto vertex = TestVertex("!");
        auto child_a = vertex.insert(1);
        auto child_b = vertex.insert(1);
        EXPECT_TRUE(child_a.second);
        EXPECT_FALSE(child_b.second);
        EXPECT_EQ(child_a.first, child_b.first); // same unique child
        EXPECT_NE(vertex.end(), child_a.first);
    }
}

} // namespace vertex
