// Copied (almost) directly from https://github.com/hanumantmk/cexpr_bson/blob/master/src/cexpr/data_view.hpp

#pragma once
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <string_view>

#include "cppystruct/string.h"


namespace pystruct {

template <typename T>
class data_view {
public:
   constexpr data_view(T* b, bool bigEndian) : bytes(b), isBigEndian(bigEndian) {}

   // Store

   constexpr void store(char v) {
	   bytes[0] = v;
   }

   constexpr void store(uint16_t v) {
	   if (!isBigEndian) {
		   bytes[0] = v & 0xFF;
		   bytes[1] = (v >> 8) & 0xFF;
	   } else {
		   bytes[1] = v & 0xFF;
		   bytes[0] = (v >> 8) & 0xFF;
	   }
   }

   constexpr void store(uint32_t v) {
	   if (!isBigEndian) {
		   bytes[0] = v & 0xFF;
		   bytes[1] = (v >> 8) & 0xFF;
		   bytes[2] = (v >> 16) & 0xFF;
		   bytes[3] = (v >> 24) & 0xFF;
	   } else {
		   bytes[3] = v & 0xFF;
		   bytes[2] = (v >> 8) & 0xFF;
		   bytes[1] = (v >> 16) & 0xFF;
		   bytes[0] = (v >> 24) & 0xFF;
	   }
   }

   constexpr void store(uint64_t v) {
	   if (!isBigEndian) {
		   bytes[0] = v & 0xFFul;
		   bytes[1] = (v >> 8) & 0xFFul;
		   bytes[2] = (v >> 16) & 0xFFul;
		   bytes[3] = (v >> 24) & 0xFFul;
		   bytes[4] = (v >> 32) & 0xFFul;
		   bytes[5] = (v >> 40) & 0xFFul;
		   bytes[6] = (v >> 48) & 0xFFul;
		   bytes[7] = (v >> 56) & 0xFFul;
	   } else {
		   bytes[7] = v & 0xFFul;
		   bytes[6] = (v >> 8) & 0xFFul;
		   bytes[5] = (v >> 16) & 0xFFul;
		   bytes[4] = (v >> 24) & 0xFFul;
		   bytes[3] = (v >> 32) & 0xFFul;
		   bytes[2] = (v >> 40) & 0xFFul;
		   bytes[1] = (v >> 48) & 0xFFul;
		   bytes[0] = (v >> 56) & 0xFFul;
	   }
   }

   constexpr void store(int8_t v) {
      char b = 0;

      if (v > 0) {
         b = v;
      } else {
         b = 0xFF + v + 1;
      }

      store(b);
   }

   constexpr void store(int16_t v) {
	   uint16_t b = 0;

	   if (v > 0) {
		   b = v;
	   } else {
		   b = 0xFFFF + v + 1;
	   }

	   store(b);
   }

   constexpr void store(int32_t v) {
      uint32_t b = 0;

      if (v > 0) {
         b = v;
      } else {
         b = 0xFFFFFFFF + v + 1;
      }

      store(b);
   }

   constexpr void store(int64_t v) {
      uint64_t b = 0;

      if (v > 0) {
         b = v;
      } else {
         b = 0xFFFFFFFFFFFFFFFFUL + v + 1;
      }

      store(b);
   }

   constexpr void store(double v) {
	   *(double*)bytes = v;
	   if (isBigEndian) {
		   std::reverse(bytes, bytes + sizeof(double));
	   }
   }
     
   void store(float f) {
	   *(float*)bytes = f;
	   if (isBigEndian) {
		   std::reverse(bytes, bytes + sizeof(float));
	   }
   }

   constexpr void store(const SizedString& str) {
	   for (size_t i = 0; i < str.size; i++) {
		   bytes[i] = str.data[i];
	   }
   }





   // Get
   template <typename T>
   constexpr T get();

   template <>
   constexpr char get() {
      return bytes[0];
   }

   template <>
   constexpr uint16_t get() {
      uint16_t v = 0;
      if (!isBigEndian) {
         v += bytes[0];
         v += (static_cast<uint16_t>(bytes[1]) << 8);
      } else {
         v += bytes[1];
         v += (static_cast<uint16_t>(bytes[0]) << 8);
      }

	  return v;
   }

   template <>
   constexpr uint32_t get() {
      uint32_t v = 0;
      if (!isBigEndian) {
         v += bytes[0];
         v += (static_cast<uint32_t>(bytes[1]) << 8);
         v += (static_cast<uint32_t>(bytes[2]) << 16);
         v += (static_cast<uint32_t>(bytes[3]) << 24);
      } else {
         v += bytes[3];
         v += (static_cast<uint32_t>(bytes[2]) << 8);
         v += (static_cast<uint32_t>(bytes[1]) << 16);
		 v += (static_cast<uint32_t>(bytes[0]) << 24);
      }

     return v;
   }

   template <>
   constexpr uint64_t get() {
      uint64_t v = 0;
      if (!isBigEndian) {
         v += bytes[0];
         v += (static_cast<uint64_t>(bytes[1]) << 8);
         v += (static_cast<uint64_t>(bytes[2]) << 16);
         v += (static_cast<uint64_t>(bytes[3]) << 24);
         v += (static_cast<uint64_t>(bytes[4]) << 32);
         v += (static_cast<uint64_t>(bytes[5]) << 40);
         v += (static_cast<uint64_t>(bytes[6]) << 48);
         v += (static_cast<uint64_t>(bytes[7]) << 56);
      } else { 
         v += bytes[7];
         v += (static_cast<uint64_t>(bytes[6]) << 8);
         v += (static_cast<uint64_t>(bytes[5]) << 16);
         v += (static_cast<uint64_t>(bytes[4]) << 24);
         v += (static_cast<uint64_t>(bytes[3]) << 32);
         v += (static_cast<uint64_t>(bytes[2]) << 40);
         v += (static_cast<uint64_t>(bytes[1]) << 48);
         v += (static_cast<uint64_t>(bytes[0]) << 56);
      }

     return v;
   }

   template <>
   constexpr int8_t get() {
      uint8_t b = get<char>();
      return static_cast<int8_t>(b - 0xFFULL - 1);
   }

   template <>
   constexpr int16_t get() {
      uint16_t b = get<uint16_t>();
      return static_cast<int16_t>(b - 0xFFFFULL - 1);
   }

   template <>
   constexpr int32_t get() {
      uint32_t b = get<uint32_t>();
      return static_cast<int32_t>(b - 0xFFFFFFFFULL - 1);
   }

   template <>
   constexpr int64_t get() {
      uint64_t b = get<uint64_t>();
      return static_cast<int64_t>(b - 0xFFFFFFFFFFFFFFFFULL - 1);
   }

   template <>
   double get() {
      double v = *(double*)bytes;
      if (isBigEndian) {
         std::reverse((char*)&v, (char*)&v + sizeof(double));
      }

      return v;
   }
     
   template <>
   float get() {
      float v = *(float*)bytes;
      if (isBigEndian) {
         std::reverse((char*)&v, (char*)&v + sizeof(float));
      }

      return v;
   }

   template <>
   constexpr std::string_view get() {
      return std::string_view(bytes, size);
   }

   size_t size = 0;
private:
	bool isBigEndian;
	T* bytes;
};

} // namespace pystruct