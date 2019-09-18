#pragma once
#include <vertex/link.h>
#include <ostream>

namespace vertex {

using TestLink = link<std::size_t, std::string>;
std::ostream& operator<<(std::ostream& output, const TestLink& link);

}  // namespace vertex
