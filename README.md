`cppystruct`
==========
[![Build Status](https://travis-ci.org/karkason/cppystruct.svg?branch=master)](https://travis-ci.org/karkason/cppystruct) [![Coverage Status](https://coveralls.io/repos/github/karkason/cppystruct/badge.svg?branch=master)](https://coveralls.io/github/karkason/cppystruct?branch=master)

Overview
--------

`cppystruct` is a C++17, header-only, constexpr clone of [Python's struct module](https://docs.python.org/2/library/struct.html).
Write your struct format once, and use it seamlessly across C++ and Python.

```cpp
#include "cppystruct.h"

// icmp_header can be any type that supports std::size and std::data and holds bytes
auto [type, code, checksum, p_id, sequence] = pystruct::unpack(PY_STRING('bbHHh'), icmp_header);

int leet = 1337;
auto runtimePacked = pystruct::pack(PY_STRING(">2i10s"), leet, 20, "String!");
// runtimePacked is an std::array filled with "\x00\x00\x059\x00\x00\x00\x10String!\x00\x00\x00"
// The format is "compiled" and has zero overhead in runtime

constexpr auto packed = pystruct::pack(PY_STRING("<2i10s"), 10, 20, "String!");
// packed is an std::array filled with "\x00\x01\x00\x00\x10\x00\x00\x00String!\x00\x00\x00"

```


Usage
-----

cppystruct is a C++ header-only library, you can just add `cppystruct/include` to your include path and be ready to go!
The library compiles cleanly on clang, GCC & MSVC. CI is set up for clang & MSVC.

#### Run Tests
```sh
mkdir build && cd build

# build the library & tests
cmake ..
cmake --build .

# run the tests
ctest .
```


Features
--------

- [Full support for python's struct format](https://docs.python.org/2/library/struct.html#format-strings), including:
  - Byte order (little/big/network/native endian)
  - Strings (sized strings)
  - Repeat count for basic types
- Full interface
  - pack
  - unpack
  - calcsize
- Constexpr format - zero overhead to actual structs

#### Caveats
- float & double formats are not constexpr due to no constexpr reinterpret-cast
- A macro (PY_STRING) is used for the compile-time string because the UDL version is a gcc extension
- Compile time diagnostics are not beautiful
- Pascal strings (`p` format char) and pointer values (`P` format char) aren't supported

Contributing
------------

`cppystruct` is developed on GitHub at [karkason/cppystruct](https://github.com/karkason/cppystruct).
To report an issue or send a pull request, use the
[issue tracker](https://github.com/karkason/cppystruct/issues).
