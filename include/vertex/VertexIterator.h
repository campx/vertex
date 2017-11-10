#pragma once
#include <vertex/Iterator.h>

namespace vertex
{

template <typename Traversal>
struct VertexAccessor
{
	using result_type = typename Traversal::vertex_type;
	using argument_type = Traversal;

	const result_type& operator() (const argument_type& t) const
	{
		return t.vertex();
	}
};

template<typename Traversal>
using VertexIterator = Iterator<Traversal, VertexAccessor<Traversal>>;

} // namespace vertex
