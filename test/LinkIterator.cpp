#include "Node.h"
#include "gtest/gtest.h"
#include <vertex/LinkIterator.h>

namespace vertex
{

TEST(Node, LinkIterator)
{
    /**********************
     *         ROOT       *
     *      O--O--O--O    *
     *      |  |  |  |    *
     *      1  2  3  4    *
     **********************/
    using Container = std::map<std::size_t, TestNode>;
    {
        auto data = std::string("I AM ROOT");
        auto vertices = Container();
        auto vertex = TestNode(data);
        vertices.insert(std::make_pair(1, TestNode("A")));
        vertices.insert(std::make_pair(2, TestNode("B")));
        vertices.insert(std::make_pair(3, TestNode("C")));
        vertices.insert(std::make_pair(4, TestNode("D")));
        vertex.insert(vertex.end(), TestNode::link_type(1));
        vertex.insert(vertex.end(), TestNode::link_type(2));
        vertex.insert(vertex.end(), TestNode::link_type(3));
        vertex.insert(vertex.end(), TestNode::link_type(4));

        auto inserted = false;
        auto vit = Container::iterator();
        std::tie(vit, inserted) = vertices.insert(std::make_pair(0, vertex));
        auto it = LinkIterator<Container>(&vertices, &vertex, vertex.begin());
        auto begin = std::begin(it);
        auto end = std::end(it);
        EXPECT_EQ(TestNode("A"), *begin);
        EXPECT_EQ(4, std::distance(begin, end));
        auto result = std::vector<TestNode>(begin, end);
        EXPECT_EQ((std::vector<TestNode>{TestNode("A"), TestNode("B"),
                                           TestNode("C"), TestNode("D")}),
                  result);
    }
}

} // namespace vertex
