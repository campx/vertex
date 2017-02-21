#pragma once
#include <iostream>
#include <merkle/Node.h>
#include <string>

namespace merkle
{

struct TestNode : public Node<std::string, std::size_t, TestNode>
{
    using iterator = Node<std::string, std::size_t, TestNode>::iterator;
    using Node<std::string, std::size_t, TestNode>::Node;
    value_type computeHash() const;
};

std::ostream& operator<<(std::ostream& output, const TestNode& node);

} // namespace merkle
