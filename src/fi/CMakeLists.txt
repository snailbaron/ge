configure_file(build-info.hpp.in include/fi/build-info.hpp @ONLY)

add_library(fi
    fs.cpp
    memory_mapped_file.cpp
)
target_include_directories(fi PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)
