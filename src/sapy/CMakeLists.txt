project(sapy)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
include_directories(${PROJECT_SOURCE_DIR}/src/include/sapy)
add_library(${PROJECT_NAME} STATIC ${SRC})
