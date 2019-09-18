#pragma once
#include <vertex/node.h>
#include <ostream>
#include <string>
#include "link.h"

namespace vertex {

using TestNode = node<TestLink, std::string>;
std::ostream& operator<<(std::ostream& output, const TestNode& vertex);

}  // namespace vertex
