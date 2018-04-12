#pragma once
#include <iterator>

namespace pystruct {
namespace internal {
	struct format_string {
	};
} // namespace internal
} // namespace pystruct


#define PY_STRING(s) [] { \
    struct S : pystruct::internal::format_string { \
      static constexpr auto value() { return s; } \
      static constexpr size_t size() { return std::size(s) - 1; }  \
    }; \
    return S{}; \
  }()