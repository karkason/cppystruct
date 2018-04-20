#include <string_view>
#include <array>

template <size_t ArrSize>
constexpr bool operator==(const std::array<char, ArrSize>& arr, std::string_view str)
{
    // Assumming that str is a string literal
    if (ArrSize != str.size()) {
        return false;
    }

    for(size_t i = 0; i < ArrSize; i++) {
        if(arr[i] != str[i]) {
            return false;
        }
    }

    return true;
}

template <typename F, typename S, typename F2, typename S2>
constexpr bool operator==(const std::pair<F, S>& lhs, const std::pair<F2, S2>& rhs)
{
    return (lhs.first == static_cast<F>(rhs.first)) &&
        (lhs.second == static_cast<S>(rhs.second));
}
