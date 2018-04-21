#pragma once
#include <iterator>
#include <utility>

namespace pystruct {
namespace internal {

struct format_string {
};

constexpr bool isDigit(char ch)
{
    return ch >= '0' && ch <= '9';
}

template <size_t Size>
constexpr std::pair<size_t, size_t> consumeNumber(const char (&str)[Size], size_t offset)
{
    size_t num = 0;
    size_t i = offset;
    for(; isDigit(str[i]) && i < Size; i++) {
        num = static_cast<size_t>(num*10 + (str[i] - '0'));
    }

    return {num, i};
}


} // namespace internal
} // namespace pystruct


#define PY_STRING(s) [] { \
    struct S : pystruct::internal::format_string { \
      static constexpr decltype(auto) value() { return s; } \
      static constexpr size_t size() { return std::size(value()) - 1; }  \
      static constexpr auto at(size_t i) { return value()[i]; }; \
    }; \
    return S{}; \
  }()
