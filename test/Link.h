#pragma once
#include <ostream>
#include <vertex/Link.h>

namespace vertex
{

using TestLink = Link<std::size_t, std::string>;
std::ostream& operator<<(std::ostream& output, const TestLink& link);

} // namespace vertex
