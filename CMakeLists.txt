#cmake_minimum_required(VERSION 3.15)
#project(cpp_stitching_)
#
#set(CMAKE_CXX_STANDARD 11)
#
#add_executable(cpp_stitching_ main.cpp)
cmake_minimum_required(VERSION 3.15)
project(cpp_stitching_)

set(CMAKE_CXX_STANDARD 11)

add_executable(cpp_stitching_ main.cpp CameraStreamer.hpp CameraStreamer.cpp)

find_package(OpenCV REQUIRED)
find_package(Threads)


target_link_libraries(cpp_stitching_ ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})