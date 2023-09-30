configure_file(build-info.hpp.in include/build-info.hpp @ONLY)

add_library(e e.cpp)
target_include_directories(e
    PUBLIC include
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
)
