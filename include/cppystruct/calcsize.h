#pragma once
#include <utility>
#include <tuple>

#include "cppystruct/format.h"


namespace pystruct {

// Interface
template <typename Fmt>
constexpr size_t calcsize(Fmt formatString);

// Implementation
template <typename Fmt>
constexpr size_t calcsize(Fmt)
{
    constexpr auto numItems = countItems(Fmt{});
    constexpr auto lastItem = getTypeOfItem<numItems - 1>(Fmt{});

    return getBinaryOffset<numItems - 1>(Fmt{})
        + lastItem.size;
}

} // namespace pystruct

