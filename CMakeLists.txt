cmake_minimum_required(VERSION 3.14)

project(orbit_simulation VERSION 1.0.1)

set(EXECUTABLE_NAME "${PROJECT_NAME}_v${CMAKE_PROJECT_VERSION}")

add_executable(${EXECUTABLE_NAME}
src/main.cpp
src/c_orbit.cpp 
src/window_handling.cpp
src/config.cpp
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC inc)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
target_link_libraries(${EXECUTABLE_NAME} comctl32.lib)

message("Configuring ${PROJECT_NAME} version ${CMAKE_PROJECT_VERSION}")
message("Executable name set to '${EXECUTABLE_NAME}.exe'")
