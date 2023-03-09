cmake_minimum_required(VERSION 3.22.1)
project(cpptests)


file(GLOB SOURCES
    src/*.cpp
    include/*.cpp
)

include_directories("include")
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)

find_package(fmt)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)