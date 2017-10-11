#pragma once
#include <iostream>
#include <string>
#include <vertex/Vertex.h>

namespace vertex
{

using TestVertex = Vertex<std::size_t, std::string>;

std::ostream& operator<<(std::ostream& output, const TestVertex& vertex);

} // namespace vertex
