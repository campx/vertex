#pragma once
#include "Link.h"
#include <ostream>
#include <string>
#include <vertex/Node.h>

namespace vertex
{

using TestNode = Node<TestLink, std::string>;
std::ostream& operator<<(std::ostream& output, const TestNode& vertex);

} // namespace vertex
