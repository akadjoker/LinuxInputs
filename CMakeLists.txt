
cmake_minimum_required(VERSION 3.10)


project(main)


set(CMAKE_CXX_STANDARD 17)


set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)


file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)


add_executable(main ${SOURCES})


