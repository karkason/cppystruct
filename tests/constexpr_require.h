#pragma once

#ifdef WITH_CONSTEXPR_REQUIRE
#define REQUIRE_STATIC(x) REQUIRE(x); \
                          static_assert(x, #x);
#else
#define REQUIRE_STATIC(x) REQUIRE(x);
#endif
