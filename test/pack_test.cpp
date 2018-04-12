#include <cppystruct.h>

#include <catch.hpp>

#define REQUIRE_STATIC(x) REQUIRE(x); \
						  //static_assert(x, #x);

TEST_CASE("no count, no byte order", "[cppystruct::calcsize]")
{
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("c")) == 1);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cc")) == 2);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cch")) == 4);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cchh")) == 6);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cchH")) == 6);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cchHi")) == 10);
}


TEST_CASE("count", "[cppystruct::calcsize]")
{
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("4c")) == 4);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("40c")) == 40);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("c4c")) == 5);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("2i4c3h")) == 18);
}
