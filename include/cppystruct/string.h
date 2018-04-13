#pragma once
#include <iterator>

namespace pystruct {
namespace internal {
	
struct format_string {
};

constexpr bool isDigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

template <typename T, size_t Size>
constexpr std::pair<size_t, size_t> consumeNumber(const T (&str)[Size], size_t offset)
{
	int num = 0;
	size_t i = offset;
	for(i = offset; isDigit(str[i]) && i < Size; i++) {
		if(num == 0) {
			num = str[i] - '0';
		} else {
			num = num*10 + (str[i] - '0');
		}
	}

	return {num, i};
}


} // namespace internal
} // namespace pystruct


#define PY_STRING(s) [] { \
    struct S : pystruct::internal::format_string { \
      static constexpr auto value() { return s; } \
      static constexpr size_t size() { return std::size(s) - 1; }  \
      static constexpr auto at(size_t i) { return s[i]; }; \
    }; \
    return S{}; \
  }()