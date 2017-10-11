#include "TestTree.h"
#include "gtest/gtest.h"
#include <vertex/Array.h>

namespace vertex
{

TEST(Merkle, Array)
{
    auto forest = std::make_shared<TestForest>();
    auto root = forest->insert(TestVertex("root")).first;

    auto tree = TestTree(forest, root);
    auto array = Array<TestTree>(tree);
    auto a = TestVertex("a");
    array.insert(TestVertex("a"));
    array.insert(TestVertex("b"));

    std::vector<TestVertex> results{};
    for (const auto& vertex : array)
    {
        results.push_back(vertex.second);
    }

    std::vector<TestVertex> expected{TestVertex("a"), TestVertex("b")};
    EXPECT_EQ(expected.size(), results.size());
    EXPECT_EQ(expected, results);
}

} // namespace truss
