#include "TestTree.h"
#include "gtest/gtest.h"

namespace objex
{

TEST(Merkle, Tree)
{
    auto forest = std::make_shared<TestForest>();

    /*
     * O a_0
     */
    // Create root
    const auto a_0 = TestNode("a");
    const auto b_0 = TestNode("b");
    const auto c_0 = TestNode("c");
    const auto d_0 = TestNode("d");
    const auto e_0 = TestNode("e");

    auto ps = forest->insert(TestNode("snapshots")).first;
    auto snapshots = TestTree(forest, ps);
    auto root = forest->insert(a_0).first;
    auto tree = TestTree(forest, root);
    auto parent = root;
    snapshots.insert(snapshots.root(), root);
    EXPECT_NE(root, forest->vertices().end());
    EXPECT_EQ(std::size_t(2), forest->vertices().size());
    EXPECT_EQ(std::size_t(1), forest->edges().size());

    /* S a_0    O a_1
     *          |
     *          O b_0
     */
    { // Add b_0 as a child of a_0 in the forest->rooted at a_0
        parent = root;
        auto result = tree.insert(parent, b_0);
        EXPECT_NE(parent, result);
        ASSERT_NE(tree.root(), forest->vertices().end());
        ASSERT_NE(tree.root(), parent);
        EXPECT_EQ(std::size_t(1), forest->edges().count(b_0.self_edge()));
        EXPECT_EQ(std::size_t(1), tree.root()->second.edges().size());
        EXPECT_EQ(std::size_t(4), forest->vertices().size());
        EXPECT_EQ(std::size_t(2), forest->edges().size());
        snapshots.insert(snapshots.root(), tree.root());
    }
    const auto pa_1 = tree.root();

    /* S a_0    S a_1 O a_2
     *           \   / \
     *            \ /   \
     *             O b_0 O c_0
     */
    { // Add c_0 as a child of a_1
        parent = tree.root();
        auto result = tree.insert(parent, c_0);
        EXPECT_NE(parent, result);
        ASSERT_NE(tree.root(), parent);
        EXPECT_EQ(std::size_t(2), forest->edges().count(b_0.self_edge()));
        EXPECT_EQ(std::size_t(2), tree.root()->second.edges().size());
        EXPECT_EQ(std::size_t(6), forest->vertices().size());
        EXPECT_EQ(std::size_t(5), forest->edges().size());
        snapshots.insert(snapshots.root(), tree.root());
    }
    const auto pa_2 = tree.root();

    /* S a_0    S a_1 S a_2
     *           \   / \
     *            \ /   \
     *             O b_0 O c_0
     *            /
     *       a_3 O
     *          /
     *     c_1 O---O d_0
     */
    { // Add d_0 as a child of c_0 in the graph rooted at a_2
        root = tree.root();
        parent = forest->vertices().find(c_0.self_edge());
        auto result = tree.insert(parent, d_0);
        EXPECT_NE(parent, result);
        ASSERT_NE(tree.root(), root);
        EXPECT_EQ(std::size_t(3), forest->edges().count(b_0.self_edge()));
        EXPECT_EQ(std::size_t(2), root->second.edges().size());
        EXPECT_EQ(std::size_t(9), forest->vertices().size());
        EXPECT_EQ(std::size_t(9), forest->edges().size());
        parent = result;
        snapshots.insert(snapshots.root(), tree.root());
    }
    const auto pc_1 = parent;
    const auto pa_3 = tree.root();

    /* S a_0    S a_1 S a_2
     *           \   / \
     *            \ /   \
     *             O b_0 O c_0     O e_0
     *            /               /
     *       a_3 S         O a_4 /
     *          /         / \   /
     *         /_________/   \ /
     *    c_1 O---O d_0       O b_1
     *
     */
    { // Add e_0 as a child of b_0 in the graph rooted at a_3
        root = tree.root();
        parent = forest->vertices().find(b_0.self_edge());
        auto result = tree.insert(parent, e_0);
        EXPECT_NE(result, parent);
        ASSERT_NE(root, tree.root());
        EXPECT_EQ(std::size_t(3), forest->edges().count(b_0.self_edge()));
        EXPECT_EQ(std::size_t(2), tree.root()->second.edges().size());
        EXPECT_EQ(std::size_t(1), forest->edges().count(result->first));
        EXPECT_EQ(std::size_t(1), forest->edges().count(e_0.self_edge()));
        EXPECT_EQ(std::size_t(12), forest->vertices().size());
        EXPECT_EQ(std::size_t(13), forest->edges().size());
        parent = result;
        snapshots.insert(snapshots.root(), tree.root());
    }
    const auto pb_1 = parent;

    /* S a_0          S a_2
     *               / \
     *              /   \
     *             O b_0 O c_0     O e_0
     *            /               /
     *       a_3 S         S a_4 /
     *          /         / \   /
     *         /_________/   \ /
     *    c_1 O---O d_0       O b_1
     *
     */
    { // erase the root a_1
        auto edge = pa_1->first;
        auto result = snapshots.erase(snapshots.root(), pa_1);
        EXPECT_NE(forest->vertices().end(), result);
        EXPECT_EQ(std::size_t(11), forest->vertices().size());
        EXPECT_EQ(std::size_t(12), forest->edges().size());
        EXPECT_EQ(std::size_t(2), forest->edges().count(b_0.self_edge()));
        EXPECT_EQ(forest->vertices().end(), forest->vertices().find(edge));
    }
    /* S a_0          S a_2
     *               / \
     *              /   \
     *             O b_0 O c_0
     *            /
     *       a_3 S         S a_4
     *          /         / \
     *         /_________/   \
     *    c_1 O               O b_1
     *         \   O b_2     /
     *          \ / \       /
     *       d_0 O   \     /
     *                \   /
     *                 \ /
     *                  O e_0
     */
    { // add d_0 as a child of b_1 in the subtree rooted at b_1
        tree = TestTree(forest, pb_1);
        root = tree.root();
        EXPECT_EQ("b", root->second.data());
        auto result = tree.insert(root, d_0);
        EXPECT_NE(root, result);
        EXPECT_NE(root, tree.root());
        EXPECT_EQ(std::size_t(2), forest->edges().count(d_0.self_edge()));
        EXPECT_EQ(std::size_t(1), root->second.edges().size());
        EXPECT_EQ(std::size_t(2), result->second.edges().size());
        EXPECT_EQ(std::size_t(12), forest->vertices().size());
        EXPECT_EQ(std::size_t(14), forest->edges().size());
    }
    { // repeat the above, expecting the result to be the same
        tree = TestTree(forest, pb_1);
        root = tree.root();
        auto result = tree.insert(pb_1, d_0);
        EXPECT_NE(root, result);
        EXPECT_NE(tree.root(), root);
        EXPECT_EQ(std::size_t(2), forest->edges().count(d_0.self_edge()));
        EXPECT_EQ(std::size_t(2), result->second.edges().size());
        EXPECT_EQ(std::size_t(12), forest->vertices().size());
        EXPECT_EQ(std::size_t(14), forest->edges().size());
    }
    /* S a_0          S a_2
     *               / \
     *              /   \
     *             O b_0 O c_0
     *            /
     *       a_3 S         S a_4
     *          /         / \
     *         /_________/   \
     *    c_1 O               O b_1
     *         \   O b_2     /
     *          \ / \       /
     *       d_0 O   \     /
     *                \   /
     *                 \ /
     *                  O e_0
     */
    { // remove d_0 from c_1 in the subtree rooted at a_3
        // does nothing since the resulting subtree already exists
        tree = TestTree(forest, pa_3);
        auto result =
            tree.erase(pc_1, forest->vertices().find(d_0.self_edge()));
        auto pc_0 = forest->vertices().find(c_0.self_edge());
        EXPECT_EQ(pa_2, tree.root());
        EXPECT_EQ(pc_0, result);
        EXPECT_EQ(std::size_t(2), forest->edges().count(d_0.self_edge()));
        EXPECT_EQ(std::size_t(12), forest->vertices().size());
        EXPECT_EQ(std::size_t(14), forest->edges().size());
    }
    /* S a_0          S a_2
     *               / \
     *              /   \
     *             O b_0 O c_0
     *
     *                     S a_4
     *                    / \
     *         __________/   \
     *    c_1 O               O b_1
     *         \   O b_2     /
     *          \ / \       /
     *       d_0 O   \     /
     *                \   /
     *                 \ /
     *                  O e_0
     */
    { // remove a_3
        snapshots.erase(snapshots.root(), pa_3);
        EXPECT_EQ(std::size_t(2), forest->edges().count(d_0.self_edge()));
        EXPECT_EQ(std::size_t(11), forest->vertices().size());
        EXPECT_EQ(std::size_t(11), forest->edges().size());
    }
    {
        forest->clear();
        EXPECT_EQ(std::size_t(0), forest->vertices().size());
    }
    {
        root = forest->insert(a_0).first;
        tree = TestTree(forest, root);
        parent = root;
        EXPECT_NE(root, forest->vertices().end());
        EXPECT_EQ(std::size_t(1), forest->vertices().size());
        EXPECT_EQ(std::size_t(0), forest->edges().size());
    }
    /*   O a_1
     *   |
     *   O b_0
     */
    { // Add b_0 as a child of a_0 in the forest->rooted at a_0
        parent = root;
        auto result = tree.insert(parent, b_0);
        EXPECT_NE(parent, result);
        ASSERT_NE(tree.root(), forest->vertices().end());
        ASSERT_NE(tree.root(), parent);
        EXPECT_EQ(std::size_t(1), forest->edges().count(b_0.self_edge()));
        EXPECT_EQ(std::size_t(1), tree.root()->second.edges().size());
        EXPECT_EQ(std::size_t(2), forest->vertices().size());
        EXPECT_EQ(std::size_t(1), forest->edges().size());
    }
    /*  c O O a_1
     *     \|
     *      O b_0
     */
    { // Add c_0 which has b_0 as a child
        auto edges = std::vector<std::size_t>{b_0.self_edge()};
        auto c = TestNode("c", edges);
        auto result = forest->insert(c);
        EXPECT_EQ(std::size_t(2), forest->edges().count(b_0.self_edge()));
        EXPECT_EQ(std::size_t(3), forest->vertices().size());
        EXPECT_EQ(std::size_t(2), forest->edges().size());
        parent = result.first;
    }
    auto c = parent;
    /*  O a_2
     *  |
     *  O b_1
     *  |
     *  O c
     *  |
     *  O b_0
     */
    { // Add c as a child of b_0
        auto bit = forest->vertices().find(b_0.self_edge());
        EXPECT_NE(forest->vertices().end(), bit);
        auto result = tree.insert(bit, c);
        EXPECT_NE(forest->vertices().end(), result);
        EXPECT_EQ(std::size_t(1), forest->edges().count(b_0.self_edge()));
        EXPECT_EQ(std::size_t(4), forest->vertices().size());
        EXPECT_EQ(std::size_t(3), forest->edges().size());
    }
}

} // namespace objex
