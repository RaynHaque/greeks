cmake_minimum_required(VERSION 3.28)
project(app VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)
add_library(ThreadPool STATIC ThreadPool.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ThreadPool)
target_compile_options(${PROJECT_NAME} PRIVATE -O3 -march=native -ffast-math)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
