#pragma once
#include <utility>
#include <tuple>

#include "cppystruct/format.h"


namespace pystruct {

static constexpr size_t DEFAULT_ALIGNMENT = 4; 

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
	constexpr size_t sizes[] = { BigEndianFormat<fmt.value()[Indices]>::size()... };
	
	bool shouldPad = true;
	// First format char is a format mode
	if constexpr(isFormatMode(fmt.value()[0])) {
		constexpr auto firstChar = fmt.value()[0];
		shouldPad = FormatMode<firstChar>::shouldPad();
	}

	// Calculate the size, the multiplier is the count before each format char 
	size_t size = 0;
	size_t multiplier = 1;
	for(size_t i = 0; i < std::size(sizes); i++) {
		auto currentChar = fmt.value()[i];
		auto currentSize = sizes[i];

		if(i == 0 && isFormatMode(currentChar)) {
			continue;
		}

		if (isDigit(currentChar)) {
			if (multiplier == 1) {
				multiplier = currentChar - '0';
			} else {
				multiplier = multiplier * 10 + (currentChar - '0');
			}

			continue;
		}

		// Handle padding
		auto nextSize = currentSize * multiplier;
		if (shouldPad) {
			size_t currentAlignement = ((size + nextSize) % currentSize);
			if(doesFormatAlign(currentSize) && currentAlignement != 0) {
				size += currentSize - currentAlignement;
			}
		}

		size += nextSize;
		multiplier = 1;
	}

	return size;
}

} // namespace pystruct

