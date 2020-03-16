cmake_minimum_required(VERSION 3.15)
project(LabProg C)

set(CMAKE_C_STANDARD 99)

add_executable(LabProg main.c structures/position.h structures/position.c structures/game.h structures/game.c structures/gamestructures.c structures/gamestructures.h storagestructures/quadtree.c storagestructures/quadtree.h storagestructures/matrix.c storagestructures/matrix.h structures/pieces.c structures/pieces.h graphics/graphics.c graphics/graphics.h storagestructures/linkedlist.c storagestructures/linkedlist.h communication/communications.c communication/communications.h graphics/shell.c graphics/shell.h)