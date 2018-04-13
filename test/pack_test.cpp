#include <cppystruct.h>

#include <catch.hpp>

#define REQUIRE_STATIC(x) REQUIRE(x); \
						  //static_assert(x, #x);


template <size_t ArrSize, size_t StrSize>
constexpr bool operator==(const std::array<char, ArrSize>& arr, const char (&str)[StrSize])
{
	if (ArrSize != (StrSize - 1)) {
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
	REQUIRE_STATIC(pystruct::pack(PY_STRING("ci"), '*', 0x12131415) == "*\x00\x00\x00\x15\x14\x13\x12");
	REQUIRE_STATIC(pystruct::pack(PY_STRING("bfi"), true, 0.5f, 1) == "\x01\x00\x00\x00\x00\x00\x00?\x01\x00\x00\x00");
	REQUIRE_STATIC(pystruct::pack(PY_STRING("bdi"), true, 0.5, -1) == "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xe0?\xff\xff\xff\xff");

	REQUIRE_STATIC(pystruct::pack(PY_STRING(">ci"), '*', 0x12131415) == "*\x12\x13\x14\x15");
	REQUIRE_STATIC(pystruct::pack(PY_STRING(">bfi"), true, 0.5f, 1) == "\x01?\x00\x00\x00\x00\x00\x00\x01");
	REQUIRE_STATIC(pystruct::pack(PY_STRING(">bdi"), true, 0.5, -1) == "\x01?\xe0\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff");

	REQUIRE_STATIC(pystruct::pack(PY_STRING("<ci"), '*', 0x12131415) == "*\x15\x14\x13\x12");
	REQUIRE_STATIC(pystruct::pack(PY_STRING("<bfi"), true, 0.5f, 1) == "\x01\x00\x00\x00?\x01\x00\x00\x00");
	REQUIRE_STATIC(pystruct::pack(PY_STRING("<bdi"), true, 0.5, -1) == "\x01\x00\x00\x00\x00\x00\x00\xe0?\xff\xff\xff\xff");
}
