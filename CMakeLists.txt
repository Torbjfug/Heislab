cmake_minimum_required(VERSION 3.7.2)
project(Heislab)

set(CMAKE_C_STANDARD 99)

add_executable(Heislab main.c channels.h elev.c elev.h io.c io.h)