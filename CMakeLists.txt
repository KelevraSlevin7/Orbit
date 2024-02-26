cmake_minimum_required(VERSION 3.14)

project(orbit_simulation)

# add_library(Orbit_Module 
#     src/main.cpp
#     src/c_orbit.cpp 
#     src/window_handling.cpp
# )

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/c_orbit.cpp 
    src/window_handling.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_link_libraries(${PROJECT_NAME} comctl32.lib)