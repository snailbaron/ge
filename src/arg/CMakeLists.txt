add_library(arg INTERFACE)
target_include_directories(arg INTERFACE include)
set_target_properties (arg PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

if(GE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(GE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
