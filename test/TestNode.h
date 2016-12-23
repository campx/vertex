#pragma once
#include <iostream>
#include <merkle/Node.h>
#include <string>

namespace merkle
{

struct TestNode : public Node<std::string, std::size_t, TestNode>
{
    using Node<std::string, std::size_t, TestNode>::Node;
    value_type computeHash() const;
};

} // namespace merkle
