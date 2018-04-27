#include "cppystruct.h"
#include "constexpr_require.h"

#define CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER
#include <catch.hpp>


TEST_CASE("item count without format-count", "[cppystruct::format]")
{
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("c")) == 1);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("cc")) == 2);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("cch")) == 3);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("cchh")) == 4);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("cchH")) == 4);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("cchHi")) == 5);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("cchHis")) == 6);
}

TEST_CASE("item count with format-count", "[cppystruct::format]")
{
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("4c")) == 4);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("40c")) == 40);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("c4c")) == 5);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("2i4c3h")) == 9);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("c4ci")) == 6);

    REQUIRE_STATIC(pystruct::countItems(PY_STRING("4s")) == 1);
    REQUIRE_STATIC(pystruct::countItems(PY_STRING("3c4s")) == 4);
}

TEST_CASE("item count with byte order", "[cppystruct::format]")
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
    REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("cih")).formatChar == 'c');
    REQUIRE_STATIC(pystruct::getTypeOfItem<1>(PY_STRING("cih")).formatChar == 'i');
    REQUIRE_STATIC(pystruct::getTypeOfItem<2>(PY_STRING("cih")).formatChar == 'h');
}

TEST_CASE("getTypeOfItem with format specifier", "[cppystruct::format]")
{
    REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("cih")).formatChar == 'c');
    REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("@cih")).formatChar == 'c');
    REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING(">cih")).formatChar == 'c');
    REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("<cih")).formatChar == 'c');
}


TEST_CASE("getTypeOfItem with item count", "[cppystruct::format]")
{
    REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("L2ci")).formatChar == 'L');
    REQUIRE_STATIC(pystruct::getTypeOfItem<1>(PY_STRING("L2ci")).formatChar == 'c');
    REQUIRE_STATIC(pystruct::getTypeOfItem<2>(PY_STRING("L2ci")).formatChar == 'c');
    REQUIRE_STATIC(pystruct::getTypeOfItem<3>(PY_STRING("L2ci")).formatChar == 'i');

    REQUIRE_STATIC(pystruct::getTypeOfItem<0>(PY_STRING("c3sh")).formatChar == 'c');
    REQUIRE_STATIC(pystruct::getTypeOfItem<1>(PY_STRING("c3sh")).formatChar == 's');
    REQUIRE_STATIC(pystruct::getTypeOfItem<2>(PY_STRING("c3sh")).formatChar == 'h');
}

TEST_CASE("getBinaryOffset with item count", "[cppystruct::format]")
{
    REQUIRE_STATIC(pystruct::getBinaryOffset<0>(PY_STRING("L2c5si")) == 0);
    REQUIRE_STATIC(pystruct::getBinaryOffset<1>(PY_STRING("L2c5si")) == 4);
    REQUIRE_STATIC(pystruct::getBinaryOffset<2>(PY_STRING("L2c5si")) == 5);
    REQUIRE_STATIC(pystruct::getBinaryOffset<3>(PY_STRING("L2c5si")) == 6);
    REQUIRE_STATIC(pystruct::getBinaryOffset<4>(PY_STRING("L2c5si")) == 12);

    REQUIRE_STATIC(pystruct::getBinaryOffset<0>(PY_STRING("<L2c5si")) == 0);
    REQUIRE_STATIC(pystruct::getBinaryOffset<1>(PY_STRING("<L2c5si")) == 4);
    REQUIRE_STATIC(pystruct::getBinaryOffset<2>(PY_STRING("<L2c5si")) == 5);
    REQUIRE_STATIC(pystruct::getBinaryOffset<3>(PY_STRING("<L2c5si")) == 6);
    REQUIRE_STATIC(pystruct::getBinaryOffset<4>(PY_STRING("<L2c5si")) == 11);

}
