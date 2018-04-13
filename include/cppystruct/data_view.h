// Copied (almost) directly from https://github.com/hanumantmk/cexpr_bson/blob/master/src/cexpr/data_view.hpp

#pragma once
#include <cstdint>
#include <cstring>
#include <algorithm>

#include "cppystruct/string.h"


namespace pystruct {

class data_view {
public:
   constexpr data_view(char* b, bool bigEndian) : bytes(b), isBigEndian(bigEndian) {}

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

private:
	bool isBigEndian;
	char* bytes;
};

} // namespace pystruct