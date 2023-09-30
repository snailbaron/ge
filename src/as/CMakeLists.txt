add_library(as
    coro.cpp
)
target_include_directories(as PUBLIC include)

if(GE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
