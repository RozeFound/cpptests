cmake_minimum_required(VERSION 3.22.1)
project(cpptests)

SET (CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")

file(GLOB SOURCES
    src/*.hpp
    src/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)
