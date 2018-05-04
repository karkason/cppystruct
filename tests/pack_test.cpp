#include "cppystruct.h"
#include "constexpr_require.h"
#include "constexpr_compare.h"

#define CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER
#include <catch.hpp>

using namespace std::string_view_literals;

TEST_CASE("pack sanity", "[cppystruct::pack]")
{
    // Packing floats or doubles isn't constexpr, due to no constexpr reinterpret_cast...

    REQUIRE_STATIC(pystruct::pack(PY_STRING(">ci"), '*', 0x12131415) == "*\x12\x13\x14\x15"sv);
    REQUIRE(pystruct::pack(PY_STRING(">bfi"), true, 0.5f, 1) == "\x01?\x00\x00\x00\x00\x00\x00\x01"sv);
    REQUIRE(pystruct::pack(PY_STRING(">bdi"), true, 0.5, -1) == "\x01?\xe0\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff"sv);

    REQUIRE_STATIC(pystruct::pack(PY_STRING("<ci"), '*', 0x12131415) == "*\x15\x14\x13\x12"sv);
    REQUIRE(pystruct::pack(PY_STRING("<bfi"), true, 0.5f, 1) == "\x01\x00\x00\x00?\x01\x00\x00\x00"sv);
    REQUIRE(pystruct::pack(PY_STRING("<bdi"), true, 0.5, -1) == "\x01\x00\x00\x00\x00\x00\x00\xe0?\xff\xff\xff\xff"sv);
}


TEST_CASE("pack unsigned ints", "[cppystruct::pack]")
{
    REQUIRE_STATIC(pystruct::pack(PY_STRING(">BHILQ"), 254, 65534, 4294967294UL, 4294967294UL, 18446744073709551614ULL) == "\xfe\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xff\xff\xff\xff\xfe"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("!BHILQ"), 254, 65534, 4294967294UL, 4294967294UL, 18446744073709551614ULL) == "\xfe\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xff\xff\xff\xff\xfe"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("<BHILQ"), 254, 65534, 4294967294UL, 4294967294UL, 18446744073709551614ULL) == "\xfe\xfe\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xff\xff\xff\xff"sv);
}


TEST_CASE("pack signed ints", "[cppystruct::pack]")
{
    REQUIRE_STATIC(pystruct::pack(PY_STRING(">bhilq"), -127, -32767, -2147483647, -2147483647, -9223372036854775807) == "\x81\x80\x01\x80\x00\x00\x01\x80\x00\x00\x01\x80\x00\x00\x00\x00\x00\x00\x01"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("!bhilq"), -127, -32767, -2147483647, -2147483647, -9223372036854775807) == "\x81\x80\x01\x80\x00\x00\x01\x80\x00\x00\x01\x80\x00\x00\x00\x00\x00\x00\x01"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("<bhilq"), -127, -32767, -2147483647, -2147483647, -9223372036854775807) == "\x81\x01\x80\x01\x00\x00\x80\x01\x00\x00\x80\x01\x00\x00\x00\x00\x00\x00\x80"sv);

    REQUIRE_STATIC(pystruct::pack(PY_STRING(">bhilq"), 127, 32767, 2147483647, 2147483647, 9223372036854775807) == "\x7f\x7f\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\xff\xff\xff\xff"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("!bhilq"), 127, 32767, 2147483647, 2147483647, 9223372036854775807) == "\x7f\x7f\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\xff\xff\xff\xff"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("<bhilq"), 127, 32767, 2147483647, 2147483647, 9223372036854775807) == "\x7f\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\xff\xff\xff\xff\x7f"sv);
}

TEST_CASE("pack string", "[cppystruct::pack]")
{
    REQUIRE_STATIC(pystruct::pack(PY_STRING("<5s"), "12345") == "12345"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("<h5s"), 126, "12345") == "\x7e\x00""12345"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING(">h5s"), 126, "12345") == "\x00\x7e""12345"sv);

    REQUIRE_STATIC(pystruct::pack(PY_STRING("3s"), "12345") == "123"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("6s"), "12345") == "12345\x00"sv);

    // Something complex with mixed types and repeat counts
    REQUIRE_STATIC(pystruct::pack(PY_STRING("<2c3s2H"), 'x', 'y', "zwt  __", 0x1234, 0x5678) == "xyzwt\x34\x12\x78\x56"sv);
}

TEST_CASE("pack bools", "[cppystruct::pack]")
{
    REQUIRE_STATIC(pystruct::pack(PY_STRING("?"), true) == "\x01"sv);
    REQUIRE_STATIC(pystruct::pack(PY_STRING("?"), false) == "\x00"sv);

    // Non-zero == true
    for(size_t i = 1; i < 10000; i++) {
        REQUIRE(pystruct::pack(PY_STRING("?"), i) == "\x01"sv);
    }
}
