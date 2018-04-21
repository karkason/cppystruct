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
// Store
constexpr void store(data_view<char>& d, unsigned char v) {
    d.bytes[0] = char(v & 0xFF);
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

constexpr void store(data_view<char>& d, signed char v) {
    uint8_t b = 0;

    if (v > 0) {
        b = v;
    } else {
        b = 0xFF + v + 1;
    }

    store(d, b);
}

constexpr void store(data_view<char>& d, char v) {
    if constexpr (std::is_unsigned_v<char>) {
        store(d, static_cast<uint8_t>(v));
    } else {
        store(d, static_cast<int8_t>(v));
    }
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
        b = 0xFFFFFFFFFFFFFFFF + static_cast<uint64_t>(v) + 1ull;
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
constexpr char get(const data_view<const char>& d) {
    return d.bytes[0];
}

template <>
constexpr uint16_t get(const data_view<const char>& d) {
    uint16_t v = 0;
    if (!d.isBigEndian) {
        v += d.bytes[0];
        v += (static_cast<uint16_t>(d.bytes[1]) << 8);
    } else {
        v += d.bytes[1];
        v += (static_cast<uint16_t>(d.bytes[0]) << 8);
    }

    return v;
}

template <>
constexpr uint32_t get(const data_view<const char>& d) {
    uint32_t v = 0;
    if (!d.isBigEndian) {
        v += d.bytes[0];
        v += (static_cast<uint32_t>(d.bytes[1]) << 8);
        v += (static_cast<uint32_t>(d.bytes[2]) << 16);
        v += (static_cast<uint32_t>(d.bytes[3]) << 24);
    } else {
        v += d.bytes[3];
        v += (static_cast<uint32_t>(d.bytes[2]) << 8);
        v += (static_cast<uint32_t>(d.bytes[1]) << 16);
        v += (static_cast<uint32_t>(d.bytes[0]) << 24);
    }

    return v;
}

template <>
constexpr uint64_t get(const data_view<const char>& d) {
    uint64_t v = 0;
    if (!d.isBigEndian) {
        v += d.bytes[0];
        v += (static_cast<uint64_t>(d.bytes[1]) << 8);
        v += (static_cast<uint64_t>(d.bytes[2]) << 16);
        v += (static_cast<uint64_t>(d.bytes[3]) << 24);
        v += (static_cast<uint64_t>(d.bytes[4]) << 32);
        v += (static_cast<uint64_t>(d.bytes[5]) << 40);
        v += (static_cast<uint64_t>(d.bytes[6]) << 48);
        v += (static_cast<uint64_t>(d.bytes[7]) << 56);
    } else {
       v += d.bytes[7];
       v += (static_cast<uint64_t>(d.bytes[6]) << 8);
       v += (static_cast<uint64_t>(d.bytes[5]) << 16);
       v += (static_cast<uint64_t>(d.bytes[4]) << 24);
       v += (static_cast<uint64_t>(d.bytes[3]) << 32);
       v += (static_cast<uint64_t>(d.bytes[2]) << 40);
       v += (static_cast<uint64_t>(d.bytes[1]) << 48);
       v += (static_cast<uint64_t>(d.bytes[0]) << 56);
    }

    return v;
}

template <>
constexpr int8_t get(const data_view<const char>& d) {
    uint8_t b = get<char>(d);
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

} // namespace data
} // namespace pystruct
