#pragma once
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
			|| formatChar == 'd';
	}

	template <char FormatChar>
	struct FormatMode
	{
		static_assert(isFormatMode(FormatChar), "Invalid Format Mode passed");

		static constexpr bool isBigEndian = false;
		static constexpr bool shouldPad = false;
	};

	template <>
	constexpr bool FormatMode<'@'>::shouldPad = true;

	template <>
	constexpr bool FormatMode<'>'>::isBigEndian = true;

	template <>
	constexpr bool FormatMode<'!'>::isBigEndian = true;


	template <char FormatChar>
	struct BigEndianFormat
	{
		static_assert(isFormatChar(FormatChar), "Invalid Format Char passed");
		static const size_t size;
	};

#define SET_FORMAT_CHAR(ch, s) \
    template <> \
	constexpr size_t BigEndianFormat<ch>::size = s;

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