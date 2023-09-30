set(targets
    01-simple-xy
)

foreach(target ${targets})
    add_executable(${target} ${target}.cpp)
    target_link_libraries(${target} ve)
endforeach()
