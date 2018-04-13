#pragma once
#include <utility>
#include <tuple>

#include "cppystruct/format.h"


namespace pystruct {

// Interface
template <typename Fmt>
constexpr size_t calcsize(Fmt&&);

// Implementation
template <typename Fmt>
constexpr size_t calcsize(Fmt&&) 
{
	constexpr auto numItems = countItems(Fmt{});
	return getBinaryOffset<numItems - 1>(Fmt{}) 
		+ getTypeOfItem<numItems - 1>(Fmt{}).size;
}

} // namespace pystruct

