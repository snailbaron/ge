configure_file(build-info.hpp.in include/build-info.hpp @ONLY)

add_library(gx
    box.cpp
    error.cpp
    id.cpp
    renderer.cpp
    scene.cpp
    sprite.cpp
)

target_include_directories(gx PUBLIC
    include
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)
target_link_libraries(gx PUBLIC
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)

if(GE_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
