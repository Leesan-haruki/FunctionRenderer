cmake_minimum_required(VERSION 2.8)
project(function_renderer CXX)
add_executable(function_renderer model.cpp render.cpp interaction.cpp main.cpp)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

target_link_libraries(function_renderer ${OpenCV_LIBRARIES})
