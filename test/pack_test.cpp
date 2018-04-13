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
	//REQUIRE_STATIC(pystruct::pack(PY_STRING("c"), '5') == "5");
}
