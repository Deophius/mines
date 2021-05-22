cmake_minimum_required(VERSION 3.15)

project(mines)

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mnu_test mnu_test.cpp)