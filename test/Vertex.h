#pragma once
#include "Link.h"
#include <ostream>
#include <string>
#include <vertex/Vertex.h>

namespace vertex
{

using TestVertex = Vertex<TestLink, std::string>;
std::ostream& operator<<(std::ostream& output, const TestVertex& vertex);

} // namespace vertex
