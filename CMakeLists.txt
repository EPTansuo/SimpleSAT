cmake_minimum_required(VERSION 3.15)

project(SimpleSAT CXX)

# find_package(sapy REQUIRED)
find_package(argparse REQUIRED)
#find_package(onnxruntime CONFIG REQUIRED)
#find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")


include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/src/include/sapy)

file(GLOB SOURCES "src/*.cpp")

add_compile_options(-fmacro-prefix-map=${PROJECT_SOURCE_DIR}/src/=)


add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        
)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        argparse::argparse
)
