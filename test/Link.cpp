#include "Link.h"

namespace vertex
{

std::ostream& operator<<(std::ostream& output, const TestLink& link)
{
    output << link.id();
    return output;
}

} // namespace vertex
