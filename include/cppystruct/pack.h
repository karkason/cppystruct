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
	constexpr size_t expectedItemCount = pystruct::countItems(Fmt{});
	static_assert(sizeof...(args) == expectedItemCount, "pack expected items for packing != sizeof...(args) passed");

	using ArrayType = std::array<char, pystruct::calcsize(Fmt{})>;
	ArrayType output;

	constexpr auto formatMode = pystruct::getFormatMode(Fmt{});
	pystruct::getTypeOfItem<0>(Fmt{});

	return output;
}

} // namespace pystruct
