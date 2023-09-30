add_executable(arg_test test.cpp)
target_link_libraries(arg_test PRIVATE arg PRIVATE Catch2::Catch2WithMain)
add_test(NAME arg_test COMMAND arg_test)
