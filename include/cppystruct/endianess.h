#pragma once

namespace pystruct {
namespace internal {

template <typename T>
constexpr T endianswap(const T& x)
{
	return x;
}

// swap endianness of various size integral types
constexpr uint64_t endianswap(uint64_t x)
{
	return ((x & 0xff) << 56)
		| (((x >> 8) & 0xff) << 48)
		| (((x >> 16) & 0xff) << 40)
		| (((x >> 24) & 0xff) << 32)
		| (((x >> 32) & 0xff) << 24)
		| (((x >> 40) & 0xff) << 16)
		| (((x >> 48) & 0xff) << 8)
		| ((x >> 56) & 0xff);
}

constexpr int64_t endianswap(int64_t x)
{
	return ((x & 0xff) << 56)
		| (((x >> 8) & 0xff) << 48)
		| (((x >> 16) & 0xff) << 40)
		| (((x >> 24) & 0xff) << 32)
		| (((x >> 32) & 0xff) << 24)
		| (((x >> 40) & 0xff) << 16)
		| (((x >> 48) & 0xff) << 8)
		| ((x >> 56) & 0xff);
}

constexpr uint32_t endianswap(uint32_t x)
{
	return ((x & 0xff) << 24)
		| (((x >> 8) & 0xff) << 16)
		| (((x >> 16) & 0xff) << 8)
		| ((x >> 24) & 0xff);
}

constexpr int32_t endianswap(int32_t x)
{
	return ((x & 0xff) << 24)
		| (((x >> 8) & 0xff) << 16)
		| (((x >> 16) & 0xff) << 8)
		| ((x >> 24) & 0xff);
}

constexpr uint16_t endianswap(uint16_t x)
{
	return ((x & 0xff) << 8)
		 | ((x >> 8) & 0xff);
}

constexpr int16_t endianswap(int16_t x)
{
	return ((x & 0xff) << 8)
		| ((x >> 8) & 0xff);
}

float endianswap(float x)
{
	auto mem = endianswap(*reinterpret_cast<uint32_t*>(&x));
	return *reinterpret_cast<float*>(&mem);
}

double endianswap(double x)
{
	auto mem = endianswap(*reinterpret_cast<uint64_t*>(&x));
	return *reinterpret_cast<double*>(&mem);
}

template <typename Integer>
constexpr Integer swapBytesIfBigEndian(const Integer& a, bool isBigEndian)
{
	return isBigEndian ? endianswap(a) : a;
}


} // namespace internal
} // namespace pystruct