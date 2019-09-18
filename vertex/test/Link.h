#pragma once
#include <vertex/Link.h>
#include <ostream>

namespace vertex {

using TestLink = Link<std::size_t, std::string>;
std::ostream& operator<<(std::ostream& output, const TestLink& link);

}  // namespace vertex
