cmake_minimum_required(VERSION 3.8)

project(CPPYSTRUCT CXX)

include(ExternalProject)
find_package(Git)

# creates a library CPPYSTRUCT which is an interface (header files only)
add_library(CPPYSTRUCT INTERFACE)

# determine whether this is a standalone project or included by other projects
set(CPPYSTRUCT_STANDALONE_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CPPYSTRUCT_STANDALONE_PROJECT ON)
endif ()


if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

# set the CPPYSTRUCT library to be compiled only with C++17
target_compile_features(CPPYSTRUCT INTERFACE cxx_std_17)

# on *nix systems force the use of -std=c++XX instead of -std=gnu++XX (default)
set(CMAKE_CXX_EXTENSIONS OFF)

 set(CPPYSTRUCT_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# add include folders to the library and targets that consume it
target_include_directories(CPPYSTRUCT INTERFACE
    ${CPPYSTRUCT_INCLUDE_BUILD_DIR}
)

install(
    DIRECTORY include/cppystruct
    DESTINATION include
)

option(CPPYSTRUCT_TEST "Generate tests." ${CPPYSTRUCT_STANDALONE_PROJECT})
if (CPPYSTRUCT_TEST)
    enable_testing()
    add_subdirectory(tests)
endif ()
