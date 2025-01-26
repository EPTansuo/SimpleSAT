cmake_minimum_required(VERSION 3.15)
project(simple_sat)

find_package(sapy REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} sapy::sapy)
