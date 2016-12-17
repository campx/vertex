#pragma once
#include <iostream>
#include <merkle/Node.h>
#include <string>

namespace merkle
{

struct TestNode : public Node<std::string, std::size_t, TestNode>
{
    using element_type = std::string;
    using value_type = std::hash<std::string>::result_type;

    explicit TestNode(std::string data = std::string());

    value_type computeHash() const;

    ~TestNode();
};

} // namespace merkle
