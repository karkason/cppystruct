#pragma once
#include <array>

#include "cppystruct/calcsize.h"
#include "cppystruct/endianess.h"


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
	return internal::pack(Fmt{}, std::make_index_sequence<countItems(Fmt{})>(), std::forward<Args>(args)...);
}

template <typename RepType>
int packElement(void* data, const RepType& elem)
{
	*static_cast<RepType*>(data) = elem;
	return 0;
}

template <typename Fmt, size_t... Items, typename... Args>
constexpr auto internal::pack(Fmt&&, std::index_sequence<Items...>, Args&&... args)
{
	static_assert(sizeof...(args) == sizeof...(Items), "pack expected items for packing != sizeof...(args) passed");
	constexpr auto formatMode = pystruct::getFormatMode(Fmt{});

	using ArrayType = std::array<char, pystruct::calcsize(Fmt{})>;
	ArrayType output{};

	using Types = std::tuple< pystruct::BigEndianFormat<pystruct::getTypeOfItem<Items>(Fmt{})>::RepresentedType ...>;
	Types t = std::forward_as_tuple(swapBytesIfBigEndian(std::forward<Args>(args), formatMode.isBigEndian())...);

	constexpr size_t offsets[] = { getBinaryOffset<Items>(Fmt{})... };
	int _[] = { packElement(output.data() + offsets[Items], std::get<Items>(t))... };
	(void)_; // _ is a dummy for pack expansion

	return output;
}

} // namespace pystruct
