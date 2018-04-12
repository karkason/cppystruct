#pragma once
#include <cppystruct/string.h>

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
		|| formatChar == 'd' 
		|| internal::isDigit(formatChar);
}

template <char FormatChar>
struct FormatMode
{
	static_assert(isFormatMode(FormatChar), "Invalid Format Mode passed");

	static constexpr bool isBigEndian() { return false; };
	static constexpr bool shouldPad() { return false; };
};

#define SET_FORMAT_MODE(mode, padding, bigEndian) \
    template <> struct FormatMode<mode> { \
		static constexpr bool isBigEndian() { return bigEndian; }; \
		static constexpr bool shouldPad() { return padding; }; \
	}

SET_FORMAT_MODE('@', true, false);
SET_FORMAT_MODE('>', false, true);
SET_FORMAT_MODE('!', false, true);

constexpr bool doesFormatAlign(size_t size) 
{
	return size > 1;
}

template <char FormatChar>
struct BigEndianFormat
{
	static_assert(isFormatChar(FormatChar), "Invalid Format Char passed");
	static constexpr size_t size() { return 0; }
};

#define SET_FORMAT_CHAR(ch, s) \
    template <> struct BigEndianFormat<ch> { \
		static constexpr size_t size() { return s; } \
	}

SET_FORMAT_CHAR('x', 1);
SET_FORMAT_CHAR('b', 1);
SET_FORMAT_CHAR('B', 1);
SET_FORMAT_CHAR('c', 1);
SET_FORMAT_CHAR('s', 1);

// Pascal strings are not supported ideologically
//SET_FORMAT_CHAR('p', 1); 

SET_FORMAT_CHAR('h', 2);
SET_FORMAT_CHAR('H', 2);
SET_FORMAT_CHAR('i', 4);
SET_FORMAT_CHAR('I', 4);
SET_FORMAT_CHAR('l', 4);
SET_FORMAT_CHAR('L', 4);
SET_FORMAT_CHAR('q', 8);
SET_FORMAT_CHAR('Q', 8);
SET_FORMAT_CHAR('f', 4);
SET_FORMAT_CHAR('d', 8);

} // namespace pystruct