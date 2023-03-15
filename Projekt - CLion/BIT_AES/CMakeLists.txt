cmake_minimum_required(VERSION 3.14)
project(BIT_AES C)

set(CMAKE_C_STANDARD 90)

add_executable(BIT_AES main.c encrypt_logic.c encrypt_logic.h io.c io.h resources.h main.h)