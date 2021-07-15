cmake_minimum_required(VERSION 3.15)

project(mines)

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
endif()

add_library(mines STATIC butterfly.cpp mineutils.cpp roundup.cpp felix.cpp
    accio.cpp john.cpp deter_bench.cpp)

add_executable(mnu_test mnu_test.cpp)
target_link_libraries(mnu_test mines)
add_executable(butt_test butt_test.cpp)
target_link_libraries(butt_test mines)
add_executable(sched_demo sched_demo.cpp)
target_link_libraries(sched_demo mines)
add_executable(deter_bench deter_bench.cpp)
target_link_libraries(deter_bench mines)