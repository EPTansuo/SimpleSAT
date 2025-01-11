cmake_minimum_required(VERSION 3.10)

project(SimpleSAT CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

include_directories(${PROJECT_SOURCE_DIR}/src/include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_compile_options(-fmacro-prefix-map=${PROJECT_SOURCE_DIR}/src=src)

add_executable(simple_sat ${SOURCES})



