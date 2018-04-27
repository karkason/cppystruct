// This file will test the implementation against Python's output
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

#include "cppystruct.h"

#include <limits>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <catch.hpp>

template <typename T>
std::string convertToString(const T& val)
{
    if constexpr (std::is_same_v<T, char>) {
        return "chr(" + std::to_string(val) + ")";
    } else if constexpr (std::is_integral_v<T>) {
        return std::to_string(val);
    } else {
        return Catch::StringMaker<T>::convert(val);
    }
}


template <typename Fmt, typename... Args>
std::string buildPythonScript(Fmt, const std::string& outputPath, const Args&... toPack)
{
    std::string pythonScript = R"py(
# Workaround for Catch::StringMaker<bool>
true = True
false = False

import struct
bin = struct.pack()py" + convertToString(Fmt::value());

    std::string args[] = { convertToString(toPack)... };
    for(const auto& arg : args) {
        pythonScript += ", ";
        pythonScript += arg;
    }

    pythonScript +=  R"py()
with open(r')py" + outputPath + R"py(', 'wb') as f:
    f.write(bin)
    )py";

    return pythonScript;
}


template <typename Fmt, typename... Args>
auto runPythonPack(Fmt, const Args&... toPack)
{
    // Create the python script
    std::string packedBinaryFilePath = std::tmpnam(nullptr);
    std::string pythonScript = buildPythonScript(Fmt{}, packedBinaryFilePath, toPack...);
    std::cout << pythonScript << std::endl;

    // Write the python script
    std::string pythonScriptPath = std::tmpnam(nullptr);
    auto pythonScriptFile = std::ofstream(pythonScriptPath, std::ios::binary);
    pythonScriptFile.write(pythonScript.data(), pythonScript.size());
    pythonScriptFile.close(); // Flush to disk

    // Run the python script
    std::string scriptRunCommand = "python " + pythonScriptPath;
    system(scriptRunCommand.c_str());

    // Read the python script output
    std::ifstream inputFile(packedBinaryFilePath, std::ios::binary);
    while(!inputFile.good()) {
        inputFile = std::ifstream(packedBinaryFilePath, std::ios::binary);
    }

    std::vector<char> outputBuffer((
            std::istreambuf_iterator<char>(inputFile)),
            (std::istreambuf_iterator<char>()));

    // Remove script+output files
    remove(pythonScriptPath.c_str());
    remove(packedBinaryFilePath.c_str());

    return outputBuffer;
}

template <typename Fmt, typename... Args>
void testPackAgainstPython(Fmt, const Args&... toPack)
{
    auto packed = pystruct::pack(Fmt{}, toPack...);
    auto pythonPacked = runPythonPack(Fmt{}, toPack...);

    REQUIRE(packed.size() == pythonPacked.size());
    REQUIRE(std::equal(packed.begin(), packed.end(), pythonPacked.begin()));
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
