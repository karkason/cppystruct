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

// Specifying the Big Endian format
template <char FormatChar>
struct BigEndianFormat
{
    static_assert(isFormatChar(FormatChar), "Invalid Format Char passed");
    static constexpr size_t size() { return 0; }
};

#define SET_FORMAT_CHAR(ch, s, rep_type, native_rep_type) \
    template <> struct BigEndianFormat<ch> { \
        static constexpr size_t size() { return s; } \
        static constexpr size_t nativeSize() { return sizeof(native_rep_type); } \
        using RepresentedType = rep_type; \
        using NativeRepresentedType = native_rep_type; \
    }


template <typename Fmt, char FormatChar>
using RepresentedType = std::conditional_t<Fmt::isNative(),
                                          typename BigEndianFormat<FormatChar>::NativeRepresentedType,
                                          typename BigEndianFormat<FormatChar>::RepresentedType>;

SET_FORMAT_CHAR('?', 1, bool, bool);
SET_FORMAT_CHAR('x', 1, char, char);
SET_FORMAT_CHAR('b', 1, int8_t, signed char);
SET_FORMAT_CHAR('B', 1, uint8_t, unsigned char);
SET_FORMAT_CHAR('c', 1, char, char);

// Explicitly defining string - It's size is sizeof(char)
template <>
struct BigEndianFormat<'s'> {
    static constexpr size_t size() { return sizeof(char); }
    static constexpr size_t nativeSize() { return sizeof(char); }
    using RepresentedType = std::string_view;
    using NativeRepresentedType = std::string_view;
};

// Pascal strings are not supported ideologically
//SET_FORMAT_CHAR('p', 1, ?);

SET_FORMAT_CHAR('h', 2, int16_t, short);
SET_FORMAT_CHAR('H', 2, uint16_t, unsigned short);
SET_FORMAT_CHAR('i', 4, int32_t, int);
SET_FORMAT_CHAR('I', 4, uint32_t, unsigned int);
SET_FORMAT_CHAR('l', 4, int32_t, long);
SET_FORMAT_CHAR('L', 4, uint32_t, unsigned long);
SET_FORMAT_CHAR('q', 8, int64_t, long long);
SET_FORMAT_CHAR('Q', 8, uint64_t, unsigned long long);
SET_FORMAT_CHAR('f', 4, float, float);
SET_FORMAT_CHAR('d', 8, double, double);


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

struct RawFormatType
{
	char formatChar;
	size_t repeat;

	constexpr bool isString() {
		return formatChar == 's';
	}
};

struct FormatType
{
	char formatChar;
	size_t formatSize;
	size_t size;

	constexpr bool isString() {
		return formatChar == 's';
	}
};

constexpr bool doesFormatAlign(FormatType format)
{
	return format.formatSize > 1;
}


template <typename Fmt, char FormatChar, size_t Repeat=1>
constexpr size_t getSize()
{
    if constexpr (getFormatMode(Fmt{}).isNative()) {
        return BigEndianFormat<FormatChar>::nativeSize() * Repeat;
    } else {
        return BigEndianFormat<FormatChar>::size() * Repeat;
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

template <size_t Item, size_t ArrSize>
constexpr RawFormatType getUnwrappedItem(RawFormatType (&wrappedFormats) [ArrSize])
{
	size_t currentItem = 0;
	for (size_t i = 0; i < ArrSize; i++) {
		for (size_t repeat = 0; repeat < wrappedFormats[i].repeat; repeat++) {
			auto currentType = wrappedFormats[i];
			if (currentItem == Item) {
				if (!currentType.isString()) {
					currentType.repeat = 1;
				}
				return currentType;
			}

			currentItem++;
			if (currentType.isString()) {
				break;
			}
		}
	}

    // cannot get here, Item < ArrSize
	return {0, 0};
}

template <size_t Item, typename Fmt, size_t... Is>
constexpr RawFormatType getTypeOfItem(std::index_sequence<Is...>)
{
    constexpr char fomratString[] = { Fmt::at(Is)... };
	RawFormatType wrappedTypes[countItems(Fmt{})]{};

    size_t currentType = 0;
    for(size_t i = 0; i < sizeof...(Is); i++) {
        if(isFormatMode(fomratString[i])) {
            continue;
        }

        auto repeatCount = internal::consumeNumber(fomratString, i);
        i = repeatCount.second;

        wrappedTypes[currentType].formatChar = fomratString[i];
        wrappedTypes[currentType].repeat = repeatCount.first;
        if(repeatCount.first == 0) {
            wrappedTypes[currentType].repeat = 1;
        }

		currentType++;
    }

    return getUnwrappedItem<Item>(wrappedTypes);
}

template <size_t Item, typename Fmt>
constexpr FormatType getTypeOfItem(Fmt)
{
	static_assert(Item < countItems(Fmt{}), "Item requested must be inside the format");
    constexpr RawFormatType format = getTypeOfItem<Item, Fmt>(std::make_index_sequence<Fmt::size()>());

	constexpr FormatType sizedFormat = { format.formatChar,
										 getSize<Fmt, format.formatChar>(),
										 getSize<Fmt, format.formatChar, format.repeat>()};

	return sizedFormat;
}

template <typename Fmt, size_t... Items>
constexpr size_t getBinaryOffset(Fmt, std::index_sequence<Items...>)
{
    constexpr FormatType itemTypes[] = { getTypeOfItem<Items>(Fmt{})... };

    constexpr auto formatMode = pystruct::getFormatMode(Fmt{});

    size_t size = 0;
    for(size_t i = 0; i < sizeof...(Items) - 1; i++) {
        size += itemTypes[i].size;

        if (formatMode.shouldPad()) {
            if (doesFormatAlign(itemTypes[i+1])) {
                auto currentAlignment = (size % itemTypes[i + 1].formatSize);
                if (currentAlignment != 0) {
                    size += itemTypes[i + 1].formatSize - currentAlignment;
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
