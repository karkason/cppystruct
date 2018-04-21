// This file will test the implementation against Python's output
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

#include "cppystruct.h"

#include <limits>
#include <string>
#include <vector>
#include <iostream>

#include <catch.hpp>

template <typename T>
std::string convertToString(const T& val)
{
    if constexpr (std::is_integral_v<T> && sizeof(T) >= 4) {
        return std::to_string(val);
    } else {
        return Catch::StringMaker<T>::convert(val);
    }
}


template <typename Fmt, typename... Args>
auto runPythonPack(Fmt, const Args&... toPack)
{
    std::string args[] = { convertToString(toPack)... };

    std::string packedBinaryFilePath = std::tmpnam(nullptr);
    std::string pythonScript = R"py(
# Workaround for Catch::StringMaker<bool>
true = True
false = False

import struct
bin = struct.pack(')py" + std::string(Fmt::value()) + "'";

    for(const auto& arg : args) {
        pythonScript += ", ";
        pythonScript += arg;
    }

    pythonScript +=  R"py()
open(r')py" + packedBinaryFilePath + R"py(', 'wb').write(bin)
    )py";

    std::cout << pythonScript << std::endl;

    std::string pythonScriptPath = std::tmpnam(nullptr);
    if (FILE* pythonScriptFile = fopen(pythonScriptPath.data(), "wb+")) {
        fputs(pythonScript.data(), pythonScriptFile);
        fclose(pythonScriptFile);
    }

    system(("python " + pythonScriptPath).data());

    std::vector<char> output;
    if (FILE *fp = fopen(packedBinaryFilePath.c_str(), "rb"))
    {
        char buf[1024];
        while (size_t len = fread(buf, 1, sizeof(buf), fp)) {
            output.insert(output.end(), buf, buf + len);
        }

        fclose(fp);
        remove(packedBinaryFilePath.c_str());
    }

    return std::make_tuple(output, pythonScriptPath);
}

template <typename Fmt, typename... Args>
void testPackAgainstPython(Fmt, const Args&... toPack)
{
    auto packed = pystruct::pack(Fmt{}, toPack...);
    auto [pythonPacked, pythonScript] = runPythonPack(Fmt{}, toPack...);

    REQUIRE(packed.size() == pythonPacked.size());
    REQUIRE(std::equal(packed.begin(), packed.end(), pythonPacked.begin()));

    remove(pythonScript.c_str());
}

TEST_CASE("pack single items", "[cppystruct::binary_compat]")
{
    testPackAgainstPython(PY_STRING("?"), true);
    testPackAgainstPython(PY_STRING("c"), 'a');

    // Signed - positive
    testPackAgainstPython(PY_STRING("b"), 126);
    testPackAgainstPython(PY_STRING("h"), 126);
    testPackAgainstPython(PY_STRING("i"), 126);
    testPackAgainstPython(PY_STRING("l"), 126);
    testPackAgainstPython(PY_STRING("q"), 126);

    // Signed - negative
    testPackAgainstPython(PY_STRING("b"), -126);
    testPackAgainstPython(PY_STRING("h"), -126);
    testPackAgainstPython(PY_STRING("i"), -126);
    testPackAgainstPython(PY_STRING("l"), -126);
    testPackAgainstPython(PY_STRING("q"), -126);

    // Unsigned
    testPackAgainstPython(PY_STRING("B"), 112); // printable char
    testPackAgainstPython(PY_STRING("H"), std::numeric_limits<unsigned short>::max());
    testPackAgainstPython(PY_STRING("I"), std::numeric_limits<unsigned int>::max());
    testPackAgainstPython(PY_STRING("L"), std::numeric_limits<unsigned long>::max());
    testPackAgainstPython(PY_STRING("Q"), std::numeric_limits<unsigned long long>::max());

    // Strings
    testPackAgainstPython(PY_STRING("50s"), "This is a test");
    testPackAgainstPython(PY_STRING("500s"), "This is a test");
    testPackAgainstPython(PY_STRING("5s"), "This is a test");
}
