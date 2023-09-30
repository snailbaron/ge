add_library(ve INTERFACE)
target_include_directories(ve INTERFACE include)

if(GE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(GE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
