add_library(evening INTERFACE)
target_sources(evening INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/evening.hpp)
target_include_directories(evening INTERFACE include)

if(GE_BUILD_TESTS)
    add_subdirectory(tests)
    add_test(NAME evening_tests COMMAND evening_tests)
endif()
