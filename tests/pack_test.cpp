#include "cppystruct.h"
#include "constexpr_require.h"

#include <catch.hpp>

using namespace std::string_view_literals;


template <size_t ArrSize>
constexpr bool operator==(const std::array<char, ArrSize>& arr, std::string_view str)
{
    // Assumming that str is a string literal
	if (ArrSize != str.size()) {
		return false;
	}

	for(size_t i = 0; i < ArrSize; i++) {
		if(arr[i] != str[i]) {
			return false;
		}
	}

	return true;
}

TEST_CASE("Pack sanity", "[cppystruct::pack]")
{
    // Packing floats or doubles isn't constexpr, due to no constexpr reinterpret_cast...

	REQUIRE_STATIC(pystruct::pack(PY_STRING("ci"), '*', 0x12131415) == "*\x00\x00\x00\x15\x14\x13\x12"sv);
    REQUIRE(pystruct::pack(PY_STRING("bfi"), true, 0.5f, 1) == "\x01\x00\x00\x00\x00\x00\x00?\x01\x00\x00\x00"sv);
    REQUIRE(pystruct::pack(PY_STRING("bdi"), true, 0.5, -1) == "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xe0?\xff\xff\xff\xff"sv);

	REQUIRE_STATIC(pystruct::pack(PY_STRING(">ci"), '*', 0x12131415) == "*\x12\x13\x14\x15"sv);
    REQUIRE(pystruct::pack(PY_STRING(">bfi"), true, 0.5f, 1) == "\x01?\x00\x00\x00\x00\x00\x00\x01"sv);
    REQUIRE(pystruct::pack(PY_STRING(">bdi"), true, 0.5, -1) == "\x01?\xe0\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff"sv);

	REQUIRE_STATIC(pystruct::pack(PY_STRING("<ci"), '*', 0x12131415) == "*\x15\x14\x13\x12"sv);
    REQUIRE(pystruct::pack(PY_STRING("<bfi"), true, 0.5f, 1) == "\x01\x00\x00\x00?\x01\x00\x00\x00"sv);
    REQUIRE(pystruct::pack(PY_STRING("<bdi"), true, 0.5, -1) == "\x01\x00\x00\x00\x00\x00\x00\xe0?\xff\xff\xff\xff"sv);
}

TEST_CASE("Pack string", "[cppystruct::pack]")
{
	REQUIRE_STATIC(pystruct::pack(PY_STRING("5s"), "12345") == "12345"sv);
	REQUIRE_STATIC(pystruct::pack(PY_STRING("h5s"), 126, "12345") == "\x7e\x00""12345"sv);
	REQUIRE_STATIC(pystruct::pack(PY_STRING(">h5s"), 126, "12345") == "\x00\x7e""12345"sv);

	REQUIRE_STATIC(pystruct::pack(PY_STRING("3s"), "12345") == "123"sv);
	REQUIRE_STATIC(pystruct::pack(PY_STRING("6s"), "12345") == "12345\x00"sv);

	REQUIRE_STATIC(pystruct::pack(PY_STRING("<2c3s2H"), 'x', 'y', "zwt  __", 0x1234, 0x5678) == "xyzwt\x34\x12\x78\x56"sv);

}
