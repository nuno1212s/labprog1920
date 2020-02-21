cmake_minimum_required(VERSION 3.15)
project(LabProg C)

set(CMAKE_C_STANDARD 99)

add_executable(LabProg main.c structures/position.h structures/position.c structures/game.h structures/game.c structures/gamestructures.c structures/gamestructures.h)