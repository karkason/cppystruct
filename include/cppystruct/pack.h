#pragma once
#include <array>

#include "cppystruct/calcsize.h"
#include "cppystruct/data_view.h"


namespace pystruct {


template <typename Fmt, typename... Args>
constexpr auto pack(Fmt formatString, Args&&... args);


// Impl
namespace internal {
template <typename RepType>
constexpr int packElement(char* data, bool bigEndian, FormatType format, RepType elem)
{

    if constexpr (std::is_same_v<RepType, SizedString>) {
        // Trim the string size to the repeat count specified in the format
        elem.size = std::min(elem.size, format.size);
    } else {
        (void)format; // Unreferenced if constexpr RepType != SizedString
    }

    data_view<char> view(data, bigEndian);
    view.store(elem);
    return 0;
}

template <typename Fmt, size_t... Items, typename... Args>
constexpr auto pack(Fmt, std::index_sequence<Items...>, Args&&... args)
{
    static_assert(sizeof...(args) == sizeof...(Items), "pack expected items for packing != sizeof...(args) passed");
    constexpr auto formatMode = pystruct::getFormatMode(Fmt{});

    using ArrayType = std::array<char, pystruct::calcsize(Fmt{})>;
    ArrayType output{};

    constexpr FormatType formats[] = { pystruct::getTypeOfItem<Items>(Fmt{})... };
    using Types = std::tuple<typename pystruct::BigEndianFormat<
        formats[Items].formatChar
    >::RepresentedType ...>;

    // Convert args to a tuple of the represented types
    Types t = std::make_tuple(static_cast<std::tuple_element_t<Items, Types>>(args)...);

    constexpr size_t offsets[] = { getBinaryOffset<Items>(Fmt{})... };
    int _[] = { packElement(output.data() + offsets[Items],  formatMode.isBigEndian(), formats[Items], std::get<Items>(t))... };
    (void)_; // _ is a dummy for pack expansion

    return output;
}
} // namespace internal

template <typename Fmt, typename... Args>
constexpr auto pack(Fmt, Args&&... args)
{
    return internal::pack(Fmt{}, std::make_index_sequence<countItems(Fmt{})>(), std::forward<Args>(args)...);
}

} // namespace pystruct
