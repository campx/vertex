#include "TestTree.h"
#include "gtest/gtest.h"

namespace merkle
{

TEST(Merkle, Tree)
{
    auto forest = std::shared_ptr<TestForest>(new TestForest);

    /*
     * O a_0
     */
    // Create root node
    const auto a_0 = TestNode("a");
    const auto b_0 = TestNode("b");
    const auto c_0 = TestNode("c");
    const auto d_0 = TestNode("d");
    const auto e_0 = TestNode("e");
    auto root = forest->insert(a_0).first;
    auto tree = TestTree(forest, root);
    auto parent = root;
    EXPECT_NE(root, forest->nodes().end());
    EXPECT_EQ(std::size_t(1), forest->nodes().size());
    EXPECT_EQ(std::size_t(0), forest->links().size());
    ASSERT_EQ(forest, tree.forest());

    forest->insert(b_0);
    forest->insert(c_0);
    forest->insert(d_0);
    forest->insert(e_0);

    /* O a_0    O a_1   O c_0   O d_0   O e_0
     *          |
     *          O b_0
     */
    { // Add b_0 as a child of a_0 in the forest->rooted at a_0
        parent = root;
        auto result = tree.insert(parent, b_0);
        EXPECT_NE(parent, result);
        ASSERT_NE(tree.root(), forest->nodes().end());
        ASSERT_NE(tree.root(), parent);
        EXPECT_EQ(std::size_t(1), forest->links().count(b_0.hash()));
        EXPECT_EQ(std::size_t(1), tree.root()->second.size());
        EXPECT_EQ(std::size_t(6), forest->nodes().size());
        EXPECT_EQ(std::size_t(1), forest->links().size());
    }
    const auto pa_1 = tree.root();

    /* O a_0    O a_1 O a_2     O d_0   O e_0
     *           \   / \
     *            \ /   \
     *             O b_0 O c_0
     */
    { // Add c_0 as a child of a_1
        parent = tree.root();
        auto result = tree.insert(parent, c_0);
        EXPECT_NE(parent, result);
        ASSERT_NE(tree.root(), parent);
        EXPECT_EQ(std::size_t(2), forest->links().count(b_0.hash()));
        EXPECT_EQ(std::size_t(2), tree.root()->second.size());
        EXPECT_EQ(std::size_t(7), forest->nodes().size());
        EXPECT_EQ(std::size_t(3), forest->links().size());
    }
    const auto pa_2 = tree.root();

    /* O a_0    O a_1 O a_2             O e_0
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
        parent = forest->nodes().find(c_0.hash());
        auto result = tree.insert(parent, d_0);
        EXPECT_NE(parent, result);
        ASSERT_NE(tree.root(), root);
        EXPECT_EQ(std::size_t(3), forest->links().count(b_0.hash()));
        EXPECT_EQ(std::size_t(2), root->second.size());
        EXPECT_EQ(std::size_t(9), forest->nodes().size());
        EXPECT_EQ(std::size_t(6), forest->links().size());
        parent = result;
    }
    const auto pc_1 = parent;
    const auto pa_3 = tree.root();

    /* O a_0    O a_1 O a_2
     *           \   / \
     *            \ /   \
     *             O b_0 O c_0     O e_0
     *            /               /
     *       a_3 O         O a_4 /
     *          /         / \   /
     *         /_________/   \ /
     *    c_1 O---O d_0       O b_1
     *
     */
    { // Add e_0 as a child of b_0 in the graph rooted at a_3
        root = tree.root();
        parent = forest->nodes().find(b_0.hash());
        auto result = tree.insert(parent, e_0);
        EXPECT_NE(result, parent);
        ASSERT_NE(root, tree.root());
        EXPECT_EQ(std::size_t(3), forest->links().count(b_0.hash()));
        EXPECT_EQ(std::size_t(2), tree.root()->second.size());
        EXPECT_EQ(std::size_t(1), forest->links().count(result->first));
        EXPECT_EQ(std::size_t(1), forest->links().count(e_0.hash()));
        EXPECT_EQ(std::size_t(11), forest->nodes().size());
        EXPECT_EQ(std::size_t(9), forest->links().size());
        parent = result;
    }
    const auto pb_1 = parent;

    /* O a_0          O a_2
     *               / \
     *              /   \
     *             O b_0 O c_0     O e_0
     *            /               /
     *       a_3 O         O a_4 /
     *          /         / \   /
     *         /_________/   \ /
     *    c_1 O---O d_0       O b_1
     *
     */
    { // erase the root a_1
        auto result = forest->erase(pa_1);
        EXPECT_NE(forest->nodes().end(), result);
        EXPECT_EQ(std::size_t(10), forest->nodes().size());
        EXPECT_EQ(std::size_t(8), forest->links().size());
        EXPECT_EQ(std::size_t(2), forest->links().count(b_0.hash()));
        EXPECT_EQ(forest->nodes().end(), forest->nodes().find(pa_1->first));
    }
    /* O a_0          O a_2
     *               / \
     *              /   \
     *             O b_0 O c_0
     *            /
     *       a_3 O         O a_4
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
        EXPECT_EQ(std::size_t(2), forest->links().count(d_0.hash()));
        EXPECT_EQ(std::size_t(1), root->second.size());
        EXPECT_EQ(std::size_t(2), result->second.size());
        EXPECT_EQ(std::size_t(11), forest->nodes().size());
        EXPECT_EQ(std::size_t(10), forest->links().size());
    }
    { // repeat the above, expecting the result to be the same
        tree = TestTree(forest, pb_1);
        root = tree.root();
        auto result = tree.insert(pb_1, d_0);
        EXPECT_NE(root, result);
        EXPECT_NE(tree.root(), root);
        EXPECT_EQ(std::size_t(2), forest->links().count(d_0.hash()));
        EXPECT_EQ(std::size_t(2), result->second.size());
        EXPECT_EQ(std::size_t(11), forest->nodes().size());
        EXPECT_EQ(std::size_t(10), forest->links().size());
    }
    /* O a_0          O a_2
     *               / \
     *              /   \
     *             O b_0 O c_0
     *            /
     *       a_3 O         O a_4
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
        auto result = tree.erase(pc_1, forest->nodes().find(d_0.hash()));
        auto pc_0 = forest->nodes().find(c_0.hash());
        EXPECT_EQ(pa_2, tree.root());
        EXPECT_EQ(pc_0, result);
        EXPECT_EQ(std::size_t(2), forest->links().count(d_0.hash()));
        EXPECT_EQ(std::size_t(11), forest->nodes().size());
        EXPECT_EQ(std::size_t(10), forest->links().size());
    }
    /* O a_0          O a_2
     *               / \
     *              /   \
     *             O b_0 O c_0
     *
     *                     O a_4
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
        forest->erase(pa_3);
        EXPECT_EQ(std::size_t(2), forest->links().count(d_0.hash()));
        EXPECT_EQ(std::size_t(10), forest->nodes().size());
        EXPECT_EQ(std::size_t(8), forest->links().size());
    }
}

} // namespace merkle
