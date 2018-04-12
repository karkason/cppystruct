#include <cppystruct.h>

#include <catch.hpp>

#define REQUIRE_STATIC(x) static_assert(x, #x); REQUIRE(x); 

TEST_CASE("no count, no byte order", "[cppystruct::pack]")
{
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("c")) == 1);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cc")) == 2);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cch")) == 4);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cchh")) == 6);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cchH")) == 6);
	REQUIRE_STATIC(pystruct::calcsize(PY_STRING("cchHi")) == 10);
}
