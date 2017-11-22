#include "Vertex.h"
#include "gtest/gtest.h"
#include <vertex/LinkIterator.h>

namespace vertex
{

TEST(Vertex, LinkIterator)
{
    /**********************
     *         ROOT       *
     *      O--O--O--O    *
     *      |  |  |  |    *
     *      1  2  3  4    *
     **********************/
    using Container = std::map<std::size_t, TestVertex>;
    {
        auto data = std::string("I AM ROOT");
        auto vertices = Container();
        auto vertex = TestVertex(data);
        vertices.insert(std::make_pair(1, TestVertex("A")));
        vertices.insert(std::make_pair(2, TestVertex("B")));
        vertices.insert(std::make_pair(3, TestVertex("C")));
        vertices.insert(std::make_pair(4, TestVertex("D")));
        vertex.insert(TestVertex::link_type(1));
        vertex.insert(TestVertex::link_type(2));
        vertex.insert(TestVertex::link_type(3));
        vertex.insert(TestVertex::link_type(4));

        auto inserted = false;
        auto vit = Container::iterator();
        std::tie(vit, inserted) = vertices.insert(std::make_pair(0, vertex));
        auto it = LinkIterator<Container>(&vertices, vit, vit->second.begin());
        auto begin = std::begin(it);
        auto end = std::end(it);
        EXPECT_EQ(TestVertex("A"), *begin);
        EXPECT_EQ(4, std::distance(begin, end));
        auto result = std::vector<TestVertex>(begin, end);
        EXPECT_EQ((std::vector<TestVertex>{TestVertex("A"), TestVertex("B"),
                                           TestVertex("C"), TestVertex("D")}),
                  result);
    }
}

} // namespace vertex
