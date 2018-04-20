#include "Link.h"
#include "gtest/gtest.h"

namespace vertex
{

std::ostream& operator<<(std::ostream& output, const TestLink& link)
{
    output << link.key();
    return output;
}

TEST(vertex, Link)
{
    auto const link_a = TestLink(0, "zero");
    auto link_b = link_a;
    EXPECT_EQ(link_a, link_b);
    link_b.key(1);
    EXPECT_NE(link_a, link_b);
    EXPECT_TRUE(link_a < link_b);
    EXPECT_FALSE(link_a > link_b);
}

} // namespace vertex
