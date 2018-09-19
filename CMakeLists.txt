cmake_minimum_required(VERSION 2.8)
project(t1)
add_executable(main main.cpp)
TARGET_LINK_LIBRARIES( main
    pthread
)
