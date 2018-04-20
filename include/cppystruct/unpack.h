#pragma once
#include <array>

#include "cppystruct/calcsize.h"
#include "cppystruct/data_view.h"


namespace pystruct {


template <typename Fmt, typename Input>
constexpr auto unpack(Fmt, Input&& packedInput);

namespace internal {

template <typename Fmt, size_t... Items, typename Input>
constexpr auto unpack(Fmt, std::index_sequence<Items...>, Input&& packedInput);

} // namespace internal


template <typename Fmt, typename Input>
constexpr auto unpack(Fmt, Input&& packedInput)
{
	return internal::unpack(Fmt{}, std::make_index_sequence<countItems(Fmt{})>(), std::forward<Input>(packedInput));
}

template <size_t Item, typename UnpackedType>
constexpr auto unpackElement(const char* begin, size_t size, bool bigEndian)
{
	data_view<const char> view(begin, bigEndian);
	view.size = size;

	if constexpr(std::is_same_v<UnpackedType, SizedString>) {
		return view.get<std::string_view>();
	} else {
		return view.get<UnpackedType>();
	}
}

template <typename Fmt, size_t... Items, typename Input>
constexpr auto internal::unpack(Fmt, std::index_sequence<Items...>, Input&& packedInput)
{
	constexpr auto formatMode = pystruct::getFormatMode(Fmt{});

	constexpr FormatType formats[] = { pystruct::getTypeOfItem<Items>(Fmt{})... };
	using Types = std::tuple<typename pystruct::BigEndianFormat<
		formats[Items].formatChar
	>::RepresentedType ...>;

	constexpr size_t offsets[] = { getBinaryOffset<Items>(Fmt{})... };
	auto unpacked = std::make_tuple(unpackElement<Items, std::tuple_element_t<Items, Types>>(
		std::data(packedInput) + offsets[Items], formats[Items].size, formatMode.isBigEndian())...);

	return unpacked;
}

} // namespace pystruct
