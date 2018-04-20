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
    using Container = std::map<TestLink, TestNode>;
    {
        auto data = std::string("I AM ROOT");
        auto vertices = Container();
        auto vertex = TestNode(data);
        vertices.insert(std::make_pair(TestLink(1), TestNode("A")));
        vertices.insert(std::make_pair(TestLink(2), TestNode("B")));
        vertices.insert(std::make_pair(TestLink(3), TestNode("C")));
        vertices.insert(std::make_pair(TestLink(4), TestNode("D")));
        vertex.links().push_back(TestLink(1));
        vertex.links().push_back(TestLink(2));
        vertex.links().push_back(TestLink(3));
        vertex.links().push_back(TestLink(4));

        auto inserted = false;
        auto vit = Container::iterator();
        std::tie(vit, inserted) =
            vertices.insert(std::make_pair(TestLink(0), vertex));
        auto it =
            LinkIterator<Container>(vertices, &vertex, vertex.links().begin());
        auto begin = std::begin(it);
        auto end = std::end(it);
        EXPECT_EQ(Container::value_type(TestLink(1u), TestNode("A")), *begin);
        EXPECT_EQ(4, std::distance(begin, end));
        auto result = std::vector<Container::value_type>(begin, end);
        EXPECT_EQ((std::vector<Container::value_type>{
                      Container::value_type(TestLink(1), TestNode("A")),
                      Container::value_type(TestLink(2), TestNode("B")),
                      Container::value_type(TestLink(3), TestNode("C")),
                      Container::value_type(TestLink(4), TestNode("D"))}),
                  result);
    }
}

} // namespace vertex
