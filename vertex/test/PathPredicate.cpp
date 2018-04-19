#include "gtest/gtest.h"
#include <vertex/PathPredicate.h>

TEST(vertex, PathPredicate)
{
    using Value = std::pair<int, std::string>;
    auto one = Value{1, "one"};
    auto two = Value{2, "two"};
    auto three = Value{3, "three"};
    auto five = Value{5, "five"};
    auto eight = Value{8, "eight"};
    auto sequence = std::vector<Value>{two, three, five};
    auto begin = sequence.cbegin();
    auto end = sequence.cend();
    auto predicate =
        vertex::makePathPredicate(begin, end,
                                  [](const Value& lhs, const Value& rhs) {
                                      return lhs.first == rhs.first;
                                  });

    EXPECT_FALSE(predicate(std::make_pair(one, two)));
    EXPECT_TRUE(predicate(std::make_pair(two, three)));
    EXPECT_FALSE(predicate(std::make_pair(two, three)));
    EXPECT_TRUE(predicate(std::make_pair(three, five)));
    EXPECT_FALSE(predicate(std::make_pair(three, five)));
    EXPECT_FALSE(predicate(std::make_pair(five, eight)));
}
