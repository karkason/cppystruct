#pragma once

#ifdef TEST_CONSTEXPR
#define REQUIRE_STATIC(x) REQUIRE(x); \
                          static_assert(x, #x);
#else
#define REQUIRE_STATIC(x) REQUIRE(x);
#endif
