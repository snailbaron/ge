cmake_minimum_required(VERSION 3.24)

project(ge)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(deps)

set(GE_BUILD_TESTS TRUE CACHE BOOL "Build tests for GE")
set(GE_BUILD_EXAMPLES FALSE CACHE BOOL "Build examples for GE")

if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
    add_compile_options(/W4 /WX)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-missing-field-initializers)
endif()

add_subdirectory(src)

add_library(ge ge.cpp)
target_include_directories(ge PUBLIC include)
target_link_libraries(ge PUBLIC
    arg
    as
    e
    evening
    fi
    gx
    tempo
    thing
    ve
)
