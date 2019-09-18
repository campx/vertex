#pragma once
#include <vertex/Node.h>
#include <ostream>
#include <string>
#include "Link.h"

namespace vertex {

using TestNode = Node<TestLink, std::string>;
std::ostream& operator<<(std::ostream& output, const TestNode& vertex);

}  // namespace vertex
