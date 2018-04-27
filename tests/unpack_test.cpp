#include "cppystruct.h"
#include "constexpr_require.h"
#include "constexpr_compare.h"

#include <limits>
#include <string>

#define CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER
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

TEST_CASE("unpack floating points", "[cppystruct::unpack]")
{
    REQUIRE(pystruct::unpack(PY_STRING("2d1f"), "\x00\x00\x00\x00\x00\x88\xb3@\x00\x00\x00\x00\x00\x00\xf0\xbf\x00\x00\x00?") == std::make_tuple(5000, -1, 0.5f));
    REQUIRE(pystruct::unpack(PY_STRING(">2d1f"), "@\xb3\x88\x00\x00\x00\x00\x00\xbf\xf0\x00\x00\x00\x00\x00\x00?\x00\x00\x00") == std::make_tuple(5000, -1, 0.5f));
    REQUIRE(pystruct::unpack(PY_STRING("!2d1f"), "@\xb3\x88\x00\x00\x00\x00\x00\xbf\xf0\x00\x00\x00\x00\x00\x00?\x00\x00\x00") == std::make_tuple(5000, -1, 0.5f));
    REQUIRE(pystruct::unpack(PY_STRING("<2d1f"), "\x00\x00\x00\x00\x00\x88\xb3@\x00\x00\x00\x00\x00\x00\xf0\xbf\x00\x00\x00?") == std::make_tuple(5000, -1, 0.5f));
}

TEST_CASE("unpack unsigned ints", "[cppystruct::pack]")
{
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("BHILQ"), "\xfe\x00\xfe\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\x00\x00\x00\x00\xfe\xff\xff\xff\xff\xff\xff\xff"sv) == std::make_tuple(254, 65534, 4294967294UL, 4294967294UL, 18446744073709551614ULL));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING(">BHILQ"), "\xfe\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xff\xff\xff\xff\xfe"sv) == std::make_tuple(254, 65534, 4294967294, 4294967294UL, 18446744073709551614ULL));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("!BHILQ"), "\xfe\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xff\xff\xff\xff\xfe"sv) == std::make_tuple(254, 65534, 4294967294, 4294967294UL, 18446744073709551614ULL));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("<BHILQ"), "\xfe\xfe\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xfe\xff\xff\xff\xff\xff\xff\xff"sv) == std::make_tuple(254, 65534, 4294967294, 4294967294UL, 18446744073709551614ULL));
}

TEST_CASE("unpack signed ints", "[cppystruct::unpack]")
{
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("bhilq"), "\x81\x00\x01\x80\x01\x00\x00\x80\x01\x00\x00\x80\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x80"sv) == std::make_tuple(-127, -32767, -2147483647, -2147483647, -9223372036854775807));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING(">bhilq"), "\x81\x80\x01\x80\x00\x00\x01\x80\x00\x00\x01\x80\x00\x00\x00\x00\x00\x00\x01"sv) == std::make_tuple(-127, -32767, -2147483647, -2147483647, -9223372036854775807));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("!bhilq"), "\x81\x80\x01\x80\x00\x00\x01\x80\x00\x00\x01\x80\x00\x00\x00\x00\x00\x00\x01"sv) == std::make_tuple(-127, -32767, -2147483647, -2147483647, -9223372036854775807));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("<bhilq"), "\x81\x01\x80\x01\x00\x00\x80\x01\x00\x00\x80\x01\x00\x00\x00\x00\x00\x00\x80"sv) == std::make_tuple(-127, -32767, -2147483647, -2147483647, -9223372036854775807));

    REQUIRE_STATIC(pystruct::unpack(PY_STRING("bhilq"),  "\x7f\x00\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\x7f"sv) == std::make_tuple(127, 32767, 2147483647, 2147483647, 9223372036854775807));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING(">bhilq"), "\x7f\x7f\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\xff\xff\xff\xff"sv) == std::make_tuple(127, 32767, 2147483647, 2147483647, 9223372036854775807));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("!bhilq"), "\x7f\x7f\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\xff\xff\xff\xff"sv) == std::make_tuple(127, 32767, 2147483647, 2147483647, 9223372036854775807));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("<bhilq"), "\x7f\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\x7f\xff\xff\xff\xff\xff\xff\xff\x7f"sv) == std::make_tuple(127, 32767, 2147483647, 2147483647, 9223372036854775807));
}

TEST_CASE("unpack bools", "[cppystruct::unpack]")
{
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("?"), "\x01"sv) == std::make_tuple(true));
    REQUIRE_STATIC(pystruct::unpack(PY_STRING("?"), "\x00"sv) == std::make_tuple(false));


    // Non-zero == true
    for(char i = 1; i < std::numeric_limits<char>::max(); i++) {
        REQUIRE(pystruct::unpack(PY_STRING("?"), std::string("") + i) == std::make_tuple(true));
    }
}
