#pragma once
#include <utility>
#include <tuple>

#include "cppystruct/format.h"


namespace pystruct {

// Interface
template <typename Fmt>
constexpr size_t calcsize(Fmt&& fmt);


namespace internal {

template <typename Fmt, size_t... Indices>
constexpr size_t calcsize(Fmt&& fmt, std::index_sequence<Indices...>);

} // namespace internal


// Implementation
template <typename Fmt>
constexpr size_t calcsize(Fmt&& fmt) 
{
	return internal::calcsize(std::forward<Fmt>(fmt), std::make_index_sequence<fmt.size()>());
}

template <typename Fmt, size_t... Indices>
constexpr size_t internal::calcsize(Fmt&& fmt, std::index_sequence<Indices...>)
{
	constexpr const size_t sizes[] = { BigEndianFormat<fmt.value()[Indices]>::size... };
	
	bool isBigEndian = false;
	bool shouldPad = true;

	// First format char is a format mode 
	if constexpr(sizes[0] == 0) {
		isBigEndian = FormatMode<fmt.value()[0]>::isBigEndian;
		shouldPad = FormatMode<fmt.value()[0]>::shouldPad;
	}


	size_t size = 0;
	for(auto s : sizes) {
		size += s;
	}

	return size;
}

} // namespace pystruct

