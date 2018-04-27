#pragma once
#include "cppystruct/string.h"

#include <cstdint>
#include <string_view>


namespace pystruct {

constexpr bool isFormatMode(char formatChar)
{
    return formatChar == '<' || formatChar == '>' || formatChar == '!'
                             || formatChar == '=' || formatChar == '@';
}

constexpr bool isFormatChar(char formatChar)
{
    return isFormatMode(formatChar) || formatChar == 'x' || formatChar == 'b'
        || formatChar == 'B' || formatChar == 'c' || formatChar == 's'
        || formatChar == 'h' || formatChar == 'H' || formatChar == 'i'
        || formatChar == 'I' || formatChar == 'l' || formatChar == 'L'
        || formatChar == 'q' || formatChar == 'Q' || formatChar == 'f'
        || formatChar == 'd' || formatChar == '?'
        || internal::isDigit(formatChar);
}


// Specifying the format mode
template <char FormatChar>
struct FormatMode
{
    static_assert(isFormatMode(FormatChar), "Invalid Format Mode passed");

    static constexpr bool isBigEndian() { return false; }
    static constexpr bool shouldPad() { return false; }
    static constexpr bool isNative() { return false; }
};

#define SET_FORMAT_MODE(mode, padding, bigEndian, native) \
    template <> struct FormatMode<mode> { \
        static constexpr bool isBigEndian() { return bigEndian; }; \
        static constexpr bool shouldPad() { return padding; }; \
        static constexpr bool isNative() { return native; } \
    }

SET_FORMAT_MODE('@', true, false, true);
SET_FORMAT_MODE('>', false, true, false);
SET_FORMAT_MODE('!', false, true, false);

constexpr bool doesFormatAlign(size_t size)
{
    return size > 1;
}

struct FormatType
{
    size_t size;
    char formatChar;
};


// Specifying the Big Endian format
template <char FormatChar>
struct BigEndianFormat
{
    static_assert(isFormatChar(FormatChar), "Invalid Format Char passed");
    static constexpr size_t size() { return 0; }
};

#define SET_FORMAT_CHAR(ch, s, rep_type) \
    template <> struct BigEndianFormat<ch> { \
        static constexpr size_t size() { return s; } \
        static constexpr size_t nativeSize() { return sizeof(rep_type); } \
        using RepresentedType = rep_type; \
    }

SET_FORMAT_CHAR('?', 1, bool);
SET_FORMAT_CHAR('x', 1, char);
SET_FORMAT_CHAR('b', 1, int8_t);
SET_FORMAT_CHAR('B', 1, uint8_t);
SET_FORMAT_CHAR('c', 1, char);


// Define string_view with custom native size
template <> struct BigEndianFormat<'s'> {
    static constexpr size_t size() { return sizeof(char); }
    static constexpr size_t nativeSize() { return sizeof(char); }
    using RepresentedType = std::string_view;
};

// Pascal strings are not supported ideologically
//SET_FORMAT_CHAR('p', 1, ?);

SET_FORMAT_CHAR('h', 2, int16_t);
SET_FORMAT_CHAR('H', 2, uint16_t);
SET_FORMAT_CHAR('i', 4, int32_t);
SET_FORMAT_CHAR('I', 4, uint32_t);
SET_FORMAT_CHAR('l', 4, int32_t);
SET_FORMAT_CHAR('L', 4, uint32_t);
SET_FORMAT_CHAR('q', 8, int64_t);
SET_FORMAT_CHAR('Q', 8, uint64_t);
SET_FORMAT_CHAR('f', 4, float);
SET_FORMAT_CHAR('d', 8, double);


template <typename Fmt>
constexpr auto getFormatMode(Fmt)
{
    // First format char is a format mode
    if constexpr(isFormatMode(Fmt::at(0))) {
        constexpr auto firstChar = Fmt::at(0);
        return FormatMode<firstChar>{};
    } else {
        return FormatMode<'@'>{};
    }
}

template <typename Fmt, typename BigEndianFmt>
constexpr size_t getSize()
{
    if constexpr (getFormatMode(Fmt{}).isNative()) {
        return BigEndianFmt::nativeSize();
    } else {
        return BigEndianFmt::size();
    }
}

template <typename Fmt>
constexpr size_t countItems(Fmt)
{
    size_t itemCount = 0;

    size_t multiplier = 1;
    for(size_t i = 0; i < Fmt::size(); i++) {
        auto currentChar = Fmt::at(i);
        if(i == 0 && isFormatMode(currentChar)) {
            continue;
        }

        if (internal::isDigit(currentChar)) {
            auto numberAndOffset = internal::consumeNumber(Fmt::value(), i);

            multiplier = numberAndOffset.first;
            i = numberAndOffset.second;
            i--; // to combat the i++ in the loop
            continue;
        }

        if (currentChar == 's') {
            itemCount++;
        } else {
            itemCount += multiplier;
        }
        multiplier = 1;
    }

    return itemCount;
}

template <size_t Item, typename Fmt, size_t CurrentItem=0, size_t CurrentI=0, size_t Multiplier=1, size_t... Is>
constexpr FormatType getTypeOfItem(std::index_sequence<Is...>)
{
    if constexpr (CurrentI >= Fmt::size()) {
        return FormatType{ 0, 0 };
    } else if constexpr (CurrentI == 0 && isFormatMode(Fmt::at(0))) {
        // If the first char is a format-mode, skip it
        return getTypeOfItem<Item, Fmt, CurrentItem, CurrentI+1>(std::index_sequence<Is...>{});
    } else if constexpr (internal::isDigit(Fmt::at(CurrentI))) {
        // If the current char is a digit, consume the number, skip it and pass it as a multiplier
        constexpr char chars[] = { Fmt::at(Is)... };
        constexpr auto numberAndIndex = internal::consumeNumber(chars, CurrentI);
        return getTypeOfItem<Item, Fmt, CurrentItem, numberAndIndex.second, numberAndIndex.first>(std::index_sequence<Is...>{});
    } else {
        // If the current char is a format char, parse it
        constexpr char currentChar = Fmt::at(CurrentI);
        if constexpr (((currentChar != 's') && (Item >= CurrentItem) && (Item < (CurrentItem + Multiplier)))
            || ((currentChar == 's') && (Item == CurrentItem))) {
            if constexpr (currentChar != 's') {
                return FormatType{ getSize<Fmt, BigEndianFormat<currentChar>>(), currentChar };
            } else {
                return FormatType{ Multiplier, currentChar };
            }
        } else {
            if constexpr(currentChar != 's') {
                return getTypeOfItem<Item, Fmt, CurrentItem + Multiplier, CurrentI + 1>(std::index_sequence<Is...>{});
            } else {
                return getTypeOfItem<Item, Fmt, CurrentItem + 1, CurrentI + 1>(std::index_sequence<Is...>{});
            }
        }
    }
}

template <size_t Item, typename Fmt>
constexpr auto getTypeOfItem(Fmt)
{
    return getTypeOfItem<Item, Fmt>(std::make_index_sequence<Fmt::size()>());
}

template <typename Fmt, size_t... Items>
constexpr size_t getBinaryOffset(Fmt, std::index_sequence<Items...>)
{
    constexpr FormatType itemTypes[] = { getTypeOfItem<Items>(Fmt{})... };
    constexpr size_t formatSizes[] = { getSize<Fmt, BigEndianFormat<itemTypes[Items].formatChar>>()... };

    constexpr auto formatMode = pystruct::getFormatMode(Fmt{});

    size_t size = 0;
    for(size_t i = 0; i < sizeof...(Items) - 1; i++) {
        size += itemTypes[i].size;

        if (formatMode.shouldPad()) {
            if (doesFormatAlign(formatSizes[i+1])) {
                auto currentAlignment = (size % formatSizes[i + 1]);
                if (currentAlignment != 0) {
                    size += formatSizes[i + 1] - currentAlignment;
                }
            }
        }
    }

    return size;
}

template <size_t Item, typename Fmt>
constexpr size_t getBinaryOffset(Fmt)
{
    return getBinaryOffset(Fmt{}, std::make_index_sequence<Item+1>());
}



} // namespace pystruct
