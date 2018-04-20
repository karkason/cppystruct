cmake_minimum_required(VERSION 3.8)

project(CppystructTests CXX)

# will make visual studio generated project group files
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CATCH_CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external"
    "-DNO_SELFTEST=true"
)

if(GIT_FOUND)
    # add catch
    ExternalProject_Add(
        catch
        PREFIX ${CMAKE_BINARY_DIR}/catch
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.2.2
        CMAKE_ARGS ${CATCH_CMAKE_ARGS}
        LOG_DOWNLOAD 1
        UPDATE_DISCONNECTED 1
    )
else()
    # assume catch is installed in a system directory
    add_custom_target(catch)
endif()

# this interface adds compile options to how the tests are run
# please try to keep entries ordered =)
add_library(cppystruct_tests_config INTERFACE)
target_compile_options(cppystruct_tests_config INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:
        /EHsc
        /W4
        /WX
    >
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
        -fno-strict-aliasing
        -Wall
        -Wcast-align
        -Wconversion
        -Wctor-dtor-privacy
        -Werror
        -Wextra
        -Wno-missing-braces
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpedantic
        -Wshadow
        -Wsign-conversion
    >
)

# for tests to find the catch header
target_include_directories(cppystruct_tests_config INTERFACE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/external/include/catch>
)

file(GLOB TEST_SOURCES *.cpp)

add_executable(cppystruct_tests ${TEST_SOURCES})
target_link_libraries(cppystruct_tests
        CPPYSTRUCT
        cppystruct_tests_config
    )
add_dependencies(cppystruct_tests catch)
add_test(
      cppystruct_tests
      cppystruct_tests
)
