cmake_minimum_required(VERSION 3.15)

project(SimpleSAT CXX)

find_package(sapy REQUIRED)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/src/include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_compile_options(-fmacro-prefix-map=${PROJECT_SOURCE_DIR}/src/=)


add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} sapy::sapy)


