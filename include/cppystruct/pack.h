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
    if constexpr (std::is_same_v<RepType, std::string_view>) {
        // Trim the string size to the repeat count specified in the format
        elem = std::string_view(elem.data(), std::min(elem.size(), format.size));
    } else {
        (void)format; // Unreferenced if constexpr RepType != string_view
    }

    data_view<char> view(data, bigEndian);

    data::store(view, elem);
    return 0;
}

template <typename RepType, typename T>
constexpr RepType convert(const T& val)
{
    // If T is char[], and RepType is string_view - construct directly with std::size(val)
    //  because std::string_view doesn't have a constructor taking a char(&)[]
    if constexpr(std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>
                    && std::is_same_v<RepType, std::string_view>) {
        return RepType(std::data(val), std::size(val));
    } else {
        return static_cast<RepType>(val);
    }
}

template <typename Fmt, size_t... Items, typename... Args>
constexpr auto pack(std::index_sequence<Items...>, Args&&... args)
{
    static_assert(sizeof...(args) == sizeof...(Items), "pack expected items for packing != sizeof...(args) passed");
    constexpr auto formatMode = pystruct::getFormatMode(Fmt{});

    using ArrayType = std::array<char, pystruct::calcsize(Fmt{})>;
    ArrayType output{};

    constexpr FormatType formats[] = { pystruct::getTypeOfItem<Items>(Fmt{})... };
    using Types = std::tuple<typename pystruct::RepresentedType<
        decltype(formatMode),
        formats[Items].formatChar
    > ...>;

    // Convert args to a tuple of the represented types
    Types t = std::make_tuple(convert<std::tuple_element_t<Items, Types>>(std::forward<Args>(args))...);

    constexpr size_t offsets[] = { getBinaryOffset<Items>(Fmt{})... };
    int _[] = { 0, packElement(output.data() + offsets[Items],  formatMode.isBigEndian(), formats[Items], std::get<Items>(t))... };
    (void)_; // _ is a dummy for pack expansion

    return output;
}
} // namespace internal

template <typename Fmt, typename... Args>
constexpr auto pack(Fmt, Args&&... args)
{
    constexpr size_t itemCount = countItems(Fmt{});
    return internal::pack<Fmt>(std::make_index_sequence<itemCount>(), std::forward<Args>(args)...);
}

} // namespace pystruct
