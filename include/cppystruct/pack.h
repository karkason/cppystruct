#pragma once
#include <array>

#include "cppystruct/calcsize.h"


namespace pystruct {

template <typename Fmt, typename... Args>
constexpr auto pack(Fmt&&, Args&&... args)
{
	return std::array<char, calcsize(Fmt{})>{ args... };
}

} // namespace pystruct
