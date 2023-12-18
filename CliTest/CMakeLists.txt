cmake_minimum_required(VERSION 3.26)
project(CliTest)

set(CMAKE_CXX_STANDARD 17)

add_executable(CliTest main.cpp
        clusterInterface.cpp)
