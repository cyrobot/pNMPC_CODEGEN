cmake_minimum_required(VERSION 3.0.0)

project(Example_2D_CRANE VERSION 1.0.0)

file(GLOB_RECURSE pNMPC_SOURCES "pNMPC/src/*.cpp")
add_library(pNMPC_lib ${pNMPC_SOURCES})
target_include_directories(pNMPC_lib PUBLIC "pNMPC/include")

include_directories("pNMPC/include")

add_executable(2DCrane_example Source.cpp )

target_link_libraries (2DCrane_example PUBLIC pNMPC_lib)