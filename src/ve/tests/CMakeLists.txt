add_executable(ve-test-01-xy 01-xy.cpp)
target_link_libraries(ve-test-01-xy ve Catch2::Catch2WithMain)
add_test(NAME ve-test-01-xy COMMAND ve-test-01-xy)
