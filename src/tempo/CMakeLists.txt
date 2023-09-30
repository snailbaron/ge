cmake_minimum_required (VERSION 3.8)

project(tempo)

add_library(tempo
    frame_timer.cpp
    metronome.cpp
)

target_include_directories(tempo PUBLIC include)
set_target_properties(tempo PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
