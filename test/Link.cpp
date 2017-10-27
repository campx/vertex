#include "Link.h"

namespace vertex
{

std::ostream& operator<<(std::ostream& output, const TestLink& link)
{
    output << link.key();
    return output;
}

} // namespace vertex
