#pragma once
#include <vertex/Iterator.h>

namespace vertex
{

template <typename Traversal>
struct EdgeAccessor
{
	using argument_type = Traversal;
	using result_type = typename Traversal::edge_type;

	const result_type& operator() (const argument_type& t) const
	{
		return t.edge();
	}
};

template <typename Traversal>
using EdgeIterator = Iterator<Traversal, EdgeAccessor<Traversal>>;

} // namespace vertex
