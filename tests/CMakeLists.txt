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

function(add_test_exe name)
    add_executable(${name} ${name}.cpp main.cpp)
    target_link_libraries(${name}
        CPPYSTRUCT
        cppystruct_tests_config
    )
    add_dependencies(${name} catch)
    add_test(
      ${name}
      ${name}
    )
    # group all tests under GSL_tests
    set_property(TARGET ${name} PROPERTY FOLDER "CPPYSTRUCT_tests")
endfunction()

add_test_exe(string_test)
add_test_exe(format_test)
add_test_exe(pack_test)
add_test_exe(unpack_test)
