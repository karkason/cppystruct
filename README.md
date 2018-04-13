# cppystruct
------


cppystruct is a C++17, header-only, constexpr clone of Python's struct module. The purpose of cppstruct is to write your struct format once, and use it seamlessly across the languages. 

### Usage

```
constexpr auto packed = pystruct::pack(PY_STRING("c2h"), '*', 0x1213, 0x1415);
// packed is an std::array filled with "*\x00\x13\x12\x15\x14"

constexpr auto unpacked = pystruct::unpack(PY_STRING("c2h"), packed);
// unpacked is a std::tuple<char, uint16_t, uint16_t>
```
The output of pystrcut::pack can then be used seamlessly in Python with the same format string:
```
import struct
struct.unpack("c2h",'*\x00\x13\x12\x15\x14')
 ('*', 4627, 5141)
```


### Limitations
- float & double are not constexpr due to no constexpr reinterpret-cast
- A macro is used for the compile-time string because the UDL version is a gcc extension 
- Tests are not as comprehensive as desired
- Compile time diagnostics are not beautiful
