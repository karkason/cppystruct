cmake_minimum_required(VERSION 3.8)
project(CppystructTests CXX)

option(CPPYSTRUCT_Coverage "Build test suite with coverage information" OFF)

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
        -Wctor-dtor-privacy
        -Werror
        -Wextra
        -Wno-missing-braces
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wpedantic
        -Wshadow
    >
)

if(CPPYSTRUCT_Coverage)
    message(STATUS "Building test suite with coverage information")
    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        message(FATAL_ERROR "CPPYSTRUCT_Coverage requires GCC.")
    endif()

    # enable profiling
    target_compile_options(cppystruct_tests_config INTERFACE -lgcov --coverage -g -O0 -fprofile-arcs -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline)
    target_link_libraries(cppystruct_tests_config
                          INTERFACE gcov)

    # from https://github.com/RWTH-HPC/CMake-codecov/blob/master/cmake/FindGcov.cmake
    get_filename_component(COMPILER_PATH "${CMAKE_CXX_COMPILER}" PATH)
    string(REGEX MATCH "^[0-9]+" GCC_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
    find_program(GCOV_BIN NAMES gcov-${GCC_VERSION} gcov HINTS ${COMPILER_PATH})

    # collect all source files from the chosen include dir
    file(GLOB_RECURSE SOURCE_FILES ${CPPYSTRUCT_INCLUDE_BUILD_DIR}*.h)

    message(STATUS "Gcov bin is: ${GCOV_BIN}")

endif()

# for tests to find the catch header
target_include_directories(cppystruct_tests_config INTERFACE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/external/include/catch>
)

# create the main executable for each test. this reduces the compile time
# of each test by pre-compiling catch.
add_library(test_catch STATIC main.cpp)
target_link_libraries(test_catch
    CPPYSTRUCT
    cppystruct_tests_config
)
add_dependencies(test_catch catch)
set_property(TARGET test_catch PROPERTY FOLDER "CPPYSTRUCT_tests")

function(add_test_exe name constexpr)
    set(exe_name ${name})

   if (${constexpr})
        set(exe_name ${name}_constexpr)
    else()
        add_test_exe(${name} 1)
    endif ()

    add_executable(${exe_name} ${name}.cpp)

    if (${constexpr})
        target_compile_definitions(${exe_name}
            PUBLIC -DWITH_CONSTEXPR_REQUIRE)
    endif ()

    target_link_libraries(${exe_name}
        test_catch
    )
    add_dependencies(${exe_name} catch)
    add_test(
      ${exe_name}
      ${exe_name}
    )
    # group all tests under GSL_tests
    set_property(TARGET ${exe_name} PROPERTY FOLDER "CPPYSTRUCT_tests")
endfunction()

add_test_exe(string_test 0)
add_test_exe(format_test 0)
add_test_exe(pack_test 0)
add_test_exe(unpack_test 0)
add_test_exe(binary_compatibility_test 1)
