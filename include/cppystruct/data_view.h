// Copied (almost) directly from https://github.com/hanumantmk/cexpr_bson/blob/master/src/cexpr/data_view.hpp

#pragma once
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <string_view>

namespace pystruct {

template <typename T>
struct data_view {
    constexpr data_view(T* b, bool bigEndian) : isBigEndian(bigEndian), bytes(b) {}

    size_t size = 0;
    bool isBigEndian;
    T* bytes;
};

namespace data {

namespace impl {

// Store
constexpr void store(data_view<char>& d, uint8_t v) {
    d.bytes[0] = char(v & 0xFF);
}

constexpr void store(data_view<char>& d, bool v) {
    d.bytes[0] = static_cast<char>(v);
}

constexpr void store(data_view<char>& d, uint16_t v) {
    if (!d.isBigEndian) {
        d.bytes[0] = char(v & 0xFF);
        d.bytes[1] = char((v >> 8) & 0xFF);
    } else {
        d.bytes[1] = char(v & 0xFF);
        d.bytes[0] = char((v >> 8) & 0xFF);
    }
}

constexpr void store(data_view<char>& d, uint32_t v) {
    if (!d.isBigEndian) {
        d.bytes[0] = char(v & 0xFF);
        d.bytes[1] = char((v >> 8) & 0xFF);
        d.bytes[2] = char((v >> 16) & 0xFF);
        d.bytes[3] = char((v >> 24) & 0xFF);
    } else {
        d.bytes[3] = char(v & 0xFF);
        d.bytes[2] = char((v >> 8) & 0xFF);
        d.bytes[1] = char((v >> 16) & 0xFF);
        d.bytes[0] = char((v >> 24) & 0xFF);
    }
}

constexpr void store(data_view<char>& d, uint64_t v) {
    if (!d.isBigEndian) {
        d.bytes[0] = char(v & 0xFF);
        d.bytes[1] = char((v >> 8) & 0xFF);
        d.bytes[2] = char((v >> 16) & 0xFF);
        d.bytes[3] = char((v >> 24) & 0xFF);
        d.bytes[4] = char((v >> 32) & 0xFF);
        d.bytes[5] = char((v >> 40) & 0xFF);
        d.bytes[6] = char((v >> 48) & 0xFF);
        d.bytes[7] = char((v >> 56) & 0xFF);
    } else {
        d.bytes[7] = char(v & 0xFF);
        d.bytes[6] = char((v >> 8) & 0xFF);
        d.bytes[5] = char((v >> 16) & 0xFF);
        d.bytes[4] = char((v >> 24) & 0xFF);
        d.bytes[3] = char((v >> 32) & 0xFF);
        d.bytes[2] = char((v >> 40) & 0xFF);
        d.bytes[1] = char((v >> 48) & 0xFF);
        d.bytes[0] = char((v >> 56) & 0xFF);
    }
}

constexpr void store(data_view<char>& d, int8_t v) {
    uint8_t b = 0;

    if (v > 0) {
        b = v;
    } else {
        b = 0xFF + v + 1;
    }

    store(d, b);
}

constexpr void store(data_view<char>& d, int16_t v) {
    uint16_t b = 0;

    if (v > 0) {
        b = v;
    } else {
        b = 0xFFFF + v + 1;
    }

    store(d, b);
}

constexpr void store(data_view<char>& d, int32_t v) {
    uint32_t b = 0;

    if (v > 0) {
        b = v;
    } else {
        b = static_cast<uint32_t>(0xFFFFFFFFULL + static_cast<uint32_t>(v) + 1ull);
    }

    store(d, b);
}


#ifdef _MSC_VER
#pragma warning(disable : 4307) // Integral constant overflow warning, but it is well defined for unsigned integers...
#endif
constexpr void store(data_view<char>& d, int64_t v) {
    uint64_t b = 0;

    if (v > 0) {
        b = v;
    } else {
        b = 0xFFFFFFFFFFFFFFFFULL + static_cast<uint64_t>(v) + 1ull;
    }

    store(d, b);
}

inline void store(data_view<char>& d, double v) {
    *(double*)d.bytes = v;
    if (d.isBigEndian) {
        std::reverse(d.bytes, d.bytes + sizeof(double));
    }
}

inline void store(data_view<char>& d, float f) {
    *(float*)d.bytes = f;
    if (d.isBigEndian) {
        std::reverse(d.bytes, d.bytes + sizeof(float));
    }
}

constexpr void store(data_view<char>& d, std::string_view str) {
    for (size_t i = 0; i < str.size(); i++) {
        d.bytes[i] = str[i];
    }
}


// Get
template <typename T>
constexpr T get(const data_view<const char>& d);

template <>
constexpr uint8_t get(const data_view<const char>& d) {
    return static_cast<uint8_t>(d.bytes[0] & '\xFF');
}

template <>
constexpr bool get(const data_view<const char>& d) {
    return get<uint8_t>(d) != '\0';
}

template <>
constexpr uint16_t get(const data_view<const char>& d) {
    uint16_t v = 0;
    if (!d.isBigEndian) {
        v += static_cast<uint16_t>(static_cast<uint8_t>(d.bytes[0] & 0xFF));
        v += static_cast<uint16_t>(static_cast<uint8_t>(d.bytes[1] & 0xFF) << 8);
    } else {
        v += static_cast<uint16_t>(static_cast<uint8_t>(d.bytes[1] & 0xFF));
        v += static_cast<uint16_t>(static_cast<uint8_t>(d.bytes[0] & 0xFF) << 8);
    }

    return v;
}

template <>
constexpr uint32_t get(const data_view<const char>& d) {
    uint32_t v = 0;
    if (!d.isBigEndian) {
        v += static_cast<uint32_t>(static_cast<uint8_t>(d.bytes[0] & 0xFF));
        v += static_cast<uint32_t>(static_cast<uint8_t>(d.bytes[1] & 0xFF) << 8);
        v += static_cast<uint32_t>(static_cast<uint8_t>(d.bytes[2] & 0xFF) << 16);
        v += static_cast<uint32_t>(static_cast<uint8_t>(d.bytes[3] & 0xFF) << 24);
    } else {
        v += static_cast<uint32_t>(static_cast<uint8_t>(d.bytes[3] & 0xFF));
        v += static_cast<uint32_t>(static_cast<uint8_t>(d.bytes[2] & 0xFF) << 8);
        v += static_cast<uint32_t>(static_cast<uint8_t>(d.bytes[1] & 0xFF) << 16);
        v += static_cast<uint32_t>(static_cast<uint8_t>(d.bytes[0] & 0xFF) << 24);
    }

    return v;
}

template <>
constexpr uint64_t get(const data_view<const char>& d) {
    uint64_t v = 0;
    if (!d.isBigEndian) {
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[0] & 0xFF));
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[1] & 0xFF) << 8ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[2] & 0xFF) << 16ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[3] & 0xFF) << 24ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[4] & 0xFF) << 32ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[5] & 0xFF) << 40ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[6] & 0xFF) << 48ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[7] & 0xFF) << 56ULL);
    } else {
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[7] & 0xFF));
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[6] & 0xFF) << 8ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[5] & 0xFF) << 16ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[4] & 0xFF) << 24ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[3] & 0xFF) << 32ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[2] & 0xFF) << 40ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[1] & 0xFF) << 48ULL);
        v += static_cast<uint64_t>(static_cast<uint64_t>(d.bytes[0] & 0xFF) << 56ULL);
    }

    return v;
}

template <>
constexpr int8_t get(const data_view<const char>& d) {
    uint8_t b = get<unsigned char>(d);
    return static_cast<int8_t>(b - 0xFFULL - 1);
}

template <>
constexpr int16_t get(const data_view<const char>& d) {
    uint16_t b = get<uint16_t>(d);
    return static_cast<int16_t>(b - 0xFFFFULL - 1);
}

template <>
constexpr int32_t get(const data_view<const char>& d) {
    uint32_t b = get<uint32_t>(d);
    return static_cast<int32_t>(b - 0xFFFFFFFFULL - 1);
}

template <>
constexpr int64_t get(const data_view<const char>& d) {
    uint64_t b = get<uint64_t>(d);
    return static_cast<int64_t>(b - 0xFFFFFFFFFFFFFFFFULL - 1);
}

template <>
inline double get(const data_view<const char>& d) {
    double v = *(double*)d.bytes;
    if (d.isBigEndian) {
        std::reverse((char*)&v, (char*)&v + sizeof(double));
    }

    return v;
}

template <>
inline float get(const data_view<const char>& d) {
    float v = *(float*)d.bytes;
    if (d.isBigEndian) {
        std::reverse((char*)&v, (char*)&v + sizeof(float));
    }

    return v;
}

template <>
constexpr std::string_view get(const data_view<const char>& d) {
    return std::string_view(d.bytes, d.size);
}

template <size_t Bytes>
struct unsinged_integer;

template <>
struct unsinged_integer<1> {
    using type = uint8_t;
};

template <>
struct unsinged_integer<2> {
    using type = uint16_t;
};

template <>
struct unsinged_integer<4> {
    using type = uint32_t;
};

template <>
struct unsinged_integer<8> {
    using type = uint64_t;
};

template <typename T>
using integer_type = std::conditional_t<std::is_signed_v<T>,
                                        typename std::make_signed_t<typename unsinged_integer<sizeof(T)>::type>,
                                        typename unsinged_integer<sizeof(T)>::type>;


} // namespace impl

template <typename T>
constexpr void store(data_view<char>& d, T v) {
    if constexpr(std::is_integral_v<T>) {
        return impl::store(d, static_cast<impl::integer_type<T>>(v));
    } else {
        return impl::store(d, v);
    }
}

template <typename T>
constexpr auto get(const data_view<const char>& d) {
    if constexpr(std::is_integral_v<T> && !std::is_same_v<T, bool>) {
        return impl::get<impl::integer_type<T>>(d);
    } else {
        return impl::get<T>(d);
    }
}

} // namespace data
} // namespace pystruct
