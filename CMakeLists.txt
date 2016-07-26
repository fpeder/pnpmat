cmake_minimum_required(VERSION 2.6)
project(PNPMAT)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

find_package(OpenCL)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
link_directories(${CMAKE_BINARY_DIR}/src/lib)

set(CMAKE_CXX_FLAGS "-g -Wall")

add_subdirectory(src/lib)
add_subdirectory(src/test)
