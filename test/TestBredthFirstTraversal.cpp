#include "gtest/gtest.h"
#include <vertex/BredthFirstTraversal.h>
#include <vertex/EdgeIterator.h>
#include <vertex/Vertex.h>
#include <vertex/VertexIterator.h>

namespace
{

using TestVertex = vertex::Vertex<std::string, std::string>;
using VertexMap = std::map<std::string, TestVertex>;

std::ostream& operator<<(std::ostream& output,
                         const std::pair<std::string, std::string>& edge)
{
    output << edge.first << edge.second;
    return output;
}

std::ostream& operator<<(std::ostream& output, const TestVertex& vertex)
{
    output << vertex.data();
    return output;
}

} // namespace

namespace vertex
{

TEST(Vertex, BredthFirstTraversal)
{

    /************************
     *          A           *
     *         / \          *
     *        B   C         *
     *       /\   /\        *
     *      D  E F  G       *
     ***********************/
    auto vertices = VertexMap{};
    vertices.insert(std::make_pair("G", TestVertex("G")));
    vertices.insert(std::make_pair("F", TestVertex("F")));
    vertices.insert(std::make_pair("E", TestVertex("E")));
    vertices.insert(std::make_pair("D", TestVertex("D")));
    {
        auto vertex = TestVertex("C");
        vertex.insert("F");
        vertex.insert("G");
        vertices.insert(std::make_pair("C", vertex));
    }
    {
        auto vertex = TestVertex("B");
        vertex.insert("D");
        vertex.insert("E");
        vertices.insert(std::make_pair("B", vertex));
    }
    {
        auto vertex = TestVertex("A");
        vertex.insert("B");
        vertex.insert("C");
        vertices.insert(std::make_pair("A", vertex));
    }
    EXPECT_FALSE(vertices.empty());
    { // extract edges and vertices during a single traversal of the tree
        using Bfs = BredthFirstTraversal<VertexMap>;
        auto edge_it = EdgeIterator<Bfs>(&vertices, vertices.begin());
        auto vertex_it = VertexIterator<Bfs>(edge_it.traversal());
        EXPECT_EQ(edge_it.traversal(), vertex_it.traversal());
        EXPECT_NE(begin(edge_it), end(edge_it));
        auto edge_csv = std::ostringstream{};
        auto vertex_csv = std::ostringstream{};
        for (auto end_it = std::end(edge_it); edge_it != end_it; ++edge_it)
        {
            edge_csv << *edge_it << ", ";
            vertex_csv << *vertex_it;
        }
        EXPECT_EQ("A, AB, AC, BD, BE, CF, CG, ", edge_csv.str());
        EXPECT_EQ("ABCDEFG", vertex_csv.str());
    }

    vertices.clear();
    { // traverse empty graph
        using Bfs = BredthFirstTraversal<VertexMap>;
        auto traversal = std::make_shared<Bfs>(&vertices, vertices.begin());

        // Create an accessor which pulls out edges from a traversal
        using EdgeAccess = std::function<const Bfs::edge_type&(const Bfs&)>;
        auto accessor = EdgeAccess([](auto& t) -> auto { return t.edge(); });

        auto it = make_iterator(traversal, accessor);
        EXPECT_EQ(begin(it), end(it));
        auto os = std::ostringstream{};
        for (const auto& edge : it)
        {
            os << edge << ", ";
        }
        EXPECT_EQ("", os.str());
    }
}

} // namespace vertex
