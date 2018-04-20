#include "cppystruct.h"
#include "constexpr_require.h"
#include "constexpr_compare.h"

#include <catch.hpp>

using namespace std::string_view_literals;


TEST_CASE("unpack sanity", "[cppystruct::unpack]")
{
	REQUIRE_STATIC(equals(pystruct::unpack(PY_STRING("5s"), "12345"), std::make_tuple("12345"sv)));
	REQUIRE_STATIC(equals(pystruct::unpack(PY_STRING("h5s"), "\x7e\x00""12345"), std::make_tuple(126, "12345"sv)));
	REQUIRE_STATIC(equals(pystruct::unpack(PY_STRING(">h5s"), "\x00\x7e""12345"), std::make_tuple(126, "12345"sv)));

	REQUIRE_STATIC(equals(pystruct::unpack(PY_STRING("3s"), "123"), std::make_tuple("123"sv)));
	REQUIRE_STATIC(equals(pystruct::unpack(PY_STRING("6s"), "12345\x00"), std::make_tuple("12345\x00"sv)));

	REQUIRE_STATIC(equals(pystruct::unpack(PY_STRING("<2c3s2H"), "xyzwt\x34\x12\x78\x56"sv), std::make_tuple('x', 'y', "zwt"sv, 0x1234, 0x5678)));
}
