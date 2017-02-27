#include "TestTree.h"
#include "gtest/gtest.h"
#include <merkle/Array.h>

namespace merkle
{

TEST(Merkle, Array)
{
    auto forest = std::make_shared<TestForest>();
    auto root = forest->insert(TestNode("root")).first;

    auto tree = TestTree(forest, root);
    auto array = Array<TestTree>(tree);
    auto a = TestNode("a");
    array.insert(TestNode("a"));
    array.insert(TestNode("b"));

    std::vector<TestNode> results{};
    for (const auto& node : array)
    {
        results.push_back(node.second);
    }

    std::vector<TestNode> expected{TestNode("a"), TestNode("b")};
    EXPECT_EQ(expected.size(), results.size());
    EXPECT_EQ(expected, results);
}

} // namespace truss
