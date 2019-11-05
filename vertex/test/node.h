#pragma once
#include <vertex/pod_node.h>
#include <ostream>
#include <string>
#include "link.h"

namespace test {

using TestNode = vertex::pod_node<TestLink, std::string>;
std::ostream& operator<<(std::ostream& output, const TestNode& vertex);

}  // namespace test
