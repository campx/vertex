#include "TestForest.h"
#include "gtest/gtest.h"

namespace objex
{

TEST(Merkle, Forest)
{
    auto graph = TestForest();

    /*
     * O a_0    O b_0   O c_0   O d_0   O e_0
     */
    // Create root and orphan b-e vertices
    auto a_0 = graph.insert(TestNode("a")).first;
    auto d_0 = graph.insert(TestNode("d")).first;
    auto e_0 = graph.insert(TestNode("e")).first;
    EXPECT_EQ(std::size_t(3), graph.vertices().size());
    EXPECT_EQ(std::size_t(0), graph.edges().size());
    EXPECT_NE(graph.vertices().end(), d_0);
    EXPECT_NE(graph.vertices().end(), e_0);

    auto root = graph.vertices().find(a_0->first);
    EXPECT_NE(root, graph.vertices().end());
}

} // namespace objex
