cmake_minimum_required(VERSION 3.14)

project(orbit_simulation)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/c_orbit.cpp 
    src/window_handling.cpp
    src/config.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME} comctl32.lib)