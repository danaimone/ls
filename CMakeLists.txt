cmake_minimum_required(VERSION 3.10)
project(ls C)

set(CMAKE_C_STANDARD 99)

add_executable(ls ls.c filter.c filter.h linkedlist.h linkedlist.c)