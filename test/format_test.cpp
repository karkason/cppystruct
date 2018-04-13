#include <cppystruct.h>

#include <catch.hpp>

#define REQUIRE_STATIC(x) REQUIRE(x); \
						  static_assert(x, #x);

TEST_CASE("Item count without format-count", "[cppystruct::format]")
{
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("c")) == 1);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("cc")) == 2);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("cch")) == 3);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("cchh")) == 4);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("cchH")) == 4);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("cchHi")) == 5);
}

TEST_CASE("Item count with format-count", "[cppystruct::format]")
{
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("4c")) == 4);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("40c")) == 40);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("c4c")) == 5);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("2i4c3h")) == 9);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("c4ci")) == 6);
}

TEST_CASE("Item count with byte order", "[cppystruct::format]")
{
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("!ci")) == 2);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING(">ci")) == 2);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("<ci")) == 2);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("=ci")) == 2);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("@ci")) == 2);
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("ci")) == 2);

	// Padding sanity
	REQUIRE_STATIC(pystruct::countItems(PY_STRING("ic")) == 2);
}


TEST_CASE("getTypeOfItem without item count", "[cppystruct::format]")
{
	REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("cih")) == 'c');
	REQUIRE_STATIC(pystruct::getTypeOfItem<1>(PY_STRING("cih")) == 'i');
	REQUIRE_STATIC(pystruct::getTypeOfItem<2>(PY_STRING("cih")) == 'h');
}

TEST_CASE("getTypeOfItem with format specifier", "[cppystruct::format]")
{
	REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("cih")) == 'c');
	REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("@cih")) == 'c');
	REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING(">cih")) == 'c');
	REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("<cih")) == 'c');
}


TEST_CASE("getTypeOfItem with item count", "[cppystruct::format]")
{
	REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("L2ci")) == 'L');
	REQUIRE_STATIC(pystruct::getTypeOfItem<1>(PY_STRING("L2ci")) == 'c');
	REQUIRE_STATIC(pystruct::getTypeOfItem<2>(PY_STRING("L2ci")) == 'c');
	REQUIRE_STATIC(pystruct::getTypeOfItem<3>(PY_STRING("L2ci")) == 'i');
}

TEST_CASE("getBinaryOffset with item count", "[cppystruct::format]")
{
	REQUIRE_STATIC(pystruct::getBinaryOffset<0>(PY_STRING("L2ci")) == 0);
	REQUIRE_STATIC(pystruct::getBinaryOffset<1>(PY_STRING("L2ci")) == 4);
	REQUIRE_STATIC(pystruct::getBinaryOffset<2>(PY_STRING("L2ci")) == 5);
	REQUIRE_STATIC(pystruct::getBinaryOffset<3>(PY_STRING("L2ci")) == 8);

	REQUIRE_STATIC(pystruct::getBinaryOffset<0>(PY_STRING("<L2ci")) == 0);
	REQUIRE_STATIC(pystruct::getBinaryOffset<1>(PY_STRING("<L2ci")) == 4);
	REQUIRE_STATIC(pystruct::getBinaryOffset<2>(PY_STRING("<L2ci")) == 5);
	REQUIRE_STATIC(pystruct::getBinaryOffset<3>(PY_STRING("<L2ci")) == 6);

}
