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
	constexpr const auto firstChar = fmt.value()[0];
	constexpr const size_t sizes[] = { BigEndianFormat<fmt.value()[Indices]>::size()... };
	
	bool isBigEndian = false;
	bool shouldPad = true;

	// First format char is a format mode 
	//if constexpr(isFormatMode(firstChar)) {
	//	isBigEndian = FormatMode<firstChar>::isBigEndian;
	//	shouldPad = FormatMode<firstChar>::shouldPad;
	//}

	size_t size = 0;

	size_t multiplier = 1;
	for(size_t i = 0; i < std::size(sizes); i++) {
		auto currentChar = fmt.value()[i];
		if (isDigit(currentChar)) {
			if (multiplier == 1) {
				multiplier = currentChar - '0';
			} else {
				multiplier = multiplier * 10 + (currentChar - '0');
			}

			continue;
		}

		size += sizes[i] * multiplier;
		multiplier = 1;
	}

	return size;
}

} // namespace pystruct

