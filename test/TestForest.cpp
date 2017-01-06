#include "TestForest.h"
#include "gtest/gtest.h"

namespace merkle
{

TEST(Merkle, Forest)
{
    {
        auto graph = TestForest();

        /*
         * O a_0    O b_0   O c_0   O d_0   O e_0
         */
        // Create root and orphan b-e nodes
        auto a_0 = graph.insert(TestNode("a")).first;
        auto b_0 = graph.insert(TestNode("b")).first;
        auto c_0 = graph.insert(TestNode("c")).first;
        auto d_0 = graph.insert(TestNode("d")).first;
        auto e_0 = graph.insert(TestNode("e")).first;
        EXPECT_EQ(std::size_t(5), graph.nodes().size());
        EXPECT_EQ(std::size_t(0), graph.links().size());
        EXPECT_NE(graph.nodes().end(), d_0);
        EXPECT_NE(graph.nodes().end(), e_0);

        auto parent = a_0;
        auto root = graph.nodes().find(a_0->first);
        EXPECT_NE(root, graph.nodes().end());

        /* O a_0    O a_1   O c_0   O d_0   O e_0
         *          |
         *          O b_0
         */
        { // Add b_0 as a child of a_0 in the graph rooted at a_0
            auto result = graph.insert(root, a_0, b_0);
            std::tie(root, parent) = result;
            EXPECT_NE(a_0, parent);
            ASSERT_NE(root, graph.nodes().end());
            ASSERT_NE(root, a_0);
            EXPECT_EQ(std::size_t(1), graph.links().count(b_0->first));
            EXPECT_EQ(std::size_t(1), root->second.size());
            EXPECT_EQ(std::size_t(6), graph.nodes().size());
            EXPECT_EQ(std::size_t(1), graph.links().size());
        }
        auto a_1 = root;

        /* O a_0    O a_1 O a_2     O d_0   O e_0
         *           \   / \
         *            \ /   \
         *             O b_0 O c_0
         */
        { // Add c_0 as a child of a_1 in the graph rooted at a_1
            auto result = graph.insert(root, a_1, c_0);
            std::tie(root, parent) = result;
            EXPECT_NE(a_1, parent);
            ASSERT_NE(root, a_1);
            EXPECT_EQ(std::size_t(2), graph.links().count(b_0->first));
            EXPECT_EQ(std::size_t(2), root->second.size());
            EXPECT_EQ(std::size_t(7), graph.nodes().size());
            EXPECT_EQ(std::size_t(3), graph.links().size());
        }
        auto a_2 = root;

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
            auto result = graph.insert(root, c_0, d_0);
            std::tie(root, parent) = result;
            EXPECT_NE(c_0, parent);
            ASSERT_NE(root, a_2);
            EXPECT_EQ(std::size_t(3), graph.links().count(b_0->first));
            EXPECT_EQ(std::size_t(2), root->second.size());
            EXPECT_EQ(std::size_t(9), graph.nodes().size());
            EXPECT_EQ(std::size_t(6), graph.links().size());
        }
        auto a_3 = root;
        auto c_1 = parent;

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
            auto result = graph.insert(root, b_0, e_0);
            std::tie(root, parent) = result;
            EXPECT_NE(b_0, parent);
            ASSERT_NE(root, a_3);
            EXPECT_EQ(std::size_t(3), graph.links().count(b_0->first));
            EXPECT_EQ(std::size_t(1), a_1->second.size());
            EXPECT_EQ(std::size_t(2), a_2->second.size());
            EXPECT_EQ(std::size_t(2), a_3->second.size());
            EXPECT_EQ(std::size_t(2), root->second.size());
            EXPECT_EQ(std::size_t(1), graph.links().count(parent->first));
            EXPECT_EQ(std::size_t(1), graph.links().count(e_0->first));
            EXPECT_EQ(std::size_t(11), graph.nodes().size());
            EXPECT_EQ(std::size_t(9), graph.links().size());
        }
        auto b_1 = parent;

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
            auto result = graph.erase(a_1);
            EXPECT_NE(graph.nodes().end(), result);
            EXPECT_EQ(std::size_t(10), graph.nodes().size());
            EXPECT_EQ(std::size_t(8), graph.links().size());
            EXPECT_EQ(std::size_t(2), graph.links().count(b_0->first));
            EXPECT_EQ(graph.nodes().end(), graph.nodes().find(a_1->first));
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
            auto result = graph.insert(b_1, b_1, d_0);
            std::tie(root, parent) = result;
            EXPECT_NE(b_1, parent);
            EXPECT_NE(root, b_1);
            EXPECT_EQ(std::size_t(2), graph.links().count(d_0->first));
            EXPECT_EQ(std::size_t(2), parent->second.size());
            EXPECT_EQ(std::size_t(11), graph.nodes().size());
            EXPECT_EQ(std::size_t(10), graph.links().size());
        }
        { // repeat the above, expecting the result to be the same
            auto result = graph.insert(b_1, b_1, d_0);
            std::tie(root, parent) = result;
            EXPECT_NE(b_1, parent);
            EXPECT_NE(root, b_1);
            EXPECT_EQ(std::size_t(2), graph.links().count(d_0->first));
            EXPECT_EQ(std::size_t(2), parent->second.size());
            EXPECT_EQ(std::size_t(11), graph.nodes().size());
            EXPECT_EQ(std::size_t(10), graph.links().size());
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
            auto result = graph.erase(a_3, c_1, d_0);
            std::tie(root, parent) = result;
            EXPECT_EQ(a_2, root);
            EXPECT_EQ(c_0, parent);
            EXPECT_EQ(std::size_t(2), graph.links().count(d_0->first));
            EXPECT_EQ(std::size_t(11), graph.nodes().size());
            EXPECT_EQ(std::size_t(10), graph.links().size());
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
            graph.erase(a_3);
            EXPECT_EQ(std::size_t(2), graph.links().count(d_0->first));
            EXPECT_EQ(std::size_t(10), graph.nodes().size());
            EXPECT_EQ(std::size_t(8), graph.links().size());
        }
    }
}

} // namespace merkle
