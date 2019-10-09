#pragma once
#include <vertex/link.h>
#include <ostream>

namespace vertex {

using TestLink = link<uint64_t, std::string>;
std::ostream& operator<<(std::ostream& output, const TestLink& link);

}  // namespace vertex
