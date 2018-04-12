#pragma once
#include <array>

#include "cppystruct/calcsize.h"


namespace pystruct {


template <typename Fmt, typename... Args>
constexpr auto pack(Fmt&&, Args&&... args);

namespace internal {

template <typename Fmt, size_t... Indices, typename... Args>
constexpr auto pack(Fmt&&, std::index_sequence<Indices...>, Args&&... args);

} // namespace internal

template <typename Fmt, typename... Args>
constexpr auto pack(Fmt&&, Args&&... args)
{
	return internal::pack(Fmt{}, std::make_index_sequence<Fmt::size()>(), std::forward<Args>(args)...);
}

template <typename Fmt, size_t... Indices, typename... Args>
constexpr auto internal::pack(Fmt&&, std::index_sequence<Indices...>, Args&&... args)
{
	using ArrayType = std::array<char, pystruct::calcsize(Fmt{})>;
	ArrayType output;

	bool shouldPad = true;
	// First format char is a format mode
	if constexpr(isFormatMode(Fmt::value()[0])) {
		constexpr auto firstChar = Fmt::value()[0];
		shouldPad = FormatMode<firstChar>::shouldPad();
	}

	
	return output;
}

} // namespace pystruct
