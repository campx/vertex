#include "gtest/gtest.h"
#include <vertex/Array.h>
#include <vertex/Link.h>
#include <vertex/Node.h>

TEST(Merkle, Array)
{
    using Link = vertex::Link<unsigned char, std::string>;
    using Node = vertex::Node<Link, std::string>;
    auto root = std::make_pair(Link(0), Node("/"));
    auto other = std::make_pair(Link(1), Node("one"));
    EXPECT_NE(root, other);
    EXPECT_EQ(root, root);

    using Container = std::map<Link, Node>;
    using NodeVector = std::vector<Container::value_type>;
    using Array = objex::Array<Container>;
    auto container = Container();
    ASSERT_TRUE(container.insert(root).second);
    auto array = Array(container, container.find(root.first));
    auto elephant = Array::value_type(Link(1), Node("elephant"));
    auto bear = Array::value_type(Link(2), Node("bear"));
    auto rabbit = Array::value_type(Link(3), Node("rabbit"));
    auto lion = Array::value_type(Link(4), Node("lion"));
    array.push_back(elephant);
    EXPECT_EQ(1u, array.size());
    EXPECT_FALSE(array.empty());
    array.push_back(bear);
    array.push_back(rabbit);
    array.push_back(lion);
    auto expected = NodeVector{elephant, bear, rabbit, lion};
    auto result = NodeVector(array.begin(), array.end());
    EXPECT_EQ(expected, result);
    EXPECT_EQ(4u, array.size());
    auto bearit = std::find(array.begin(), array.end(), bear);
    EXPECT_NE(array.end(), bearit);
    EXPECT_EQ(bear, *bearit);
    array.clear();
    EXPECT_TRUE(array.empty());
    EXPECT_EQ(array.begin(), array.end());
    bearit = std::find(array.begin(), array.end(), bear);
    EXPECT_EQ(array.end(), bearit);
    auto pidgeon = Array::value_type(Link(4), Node("pidgeon"));
    array.insert(bearit, pidgeon);
    auto another_pidgeon = Array::value_type(Link(5), Node("another pidgeon"));
    array.insert(array.end(), another_pidgeon);
    EXPECT_EQ(2u, array.size());
    auto cat = Array::value_type(Link(6), Node("cat"));
    array.insert(std::find(array.begin(), array.end(), another_pidgeon), cat);
    expected = NodeVector{pidgeon, cat, another_pidgeon};
    EXPECT_EQ(expected, NodeVector(array.begin(), array.end()));
}
