add_library(thing INTERFACE)
target_include_directories(thing INTERFACE include)
target_link_libraries(thing)

if(GE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
