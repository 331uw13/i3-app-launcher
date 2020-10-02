cmake_minimum_required(VERSION 3.10)
project(i3-app-launcher)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(
    -Wall
    -Werror
    -Wextra
    -Wpedantic
    -O3
    -Wshadow)


set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(PROJECT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")


set(PROJECT_SOURCE_FILES "${PROJECT_SOURCE_DIR}main.cpp")
set(PROJECT_HEADER_FILES "${PROJECT_HEADER_DIR}config.hpp")

find_package(SFML COMPONENTS system window graphics REQUIRED)


add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_HEADER_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

