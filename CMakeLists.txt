cmake_minimum_required(VERSION 3.15)

project(mines)

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mines STATIC butterfly.cpp mineutils.cpp roundup.cpp felix.cpp
    accio.cpp john.cpp)

add_executable(mnu_test mnu_test.cpp)
target_link_libraries(mnu_test mines)
add_executable(butt_test butt_test.cpp)
target_link_libraries(butt_test mines)
add_executable(sched_demo sched_demo.cpp)
target_link_libraries(sched_demo mines)
