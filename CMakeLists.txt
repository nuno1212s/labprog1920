cmake_minimum_required(VERSION 3.15)
project(LabProg C)

set(CMAKE_C_STANDARD 99)

add_executable(LabProg main.c structures/position.h structures/position.c structures/game.h structures/game.c
        structures/gamestructures.c structures/gamestructures.h storagestructures/quadtree.c
        storagestructures/quadtree.h storagestructures/matrix.c storagestructures/matrix.h structures/pieces.c
        structures/pieces.h graphics/graphics.c graphics/graphics.h
        storagestructures/linkedlist.c storagestructures/linkedlist.h communication/communications.c
        communication/communications.h storagestructures/bitmap.c storagestructures/bitmap.h communication/text.c
        communication/text.h communication/sem.c communication/sem.h graphics/shell.c graphics/shell.h graphics/shell.c
        communication/same_shell.c communication/same_shell.h gameplay.c gameplay.h structures/gamepieces.c
        structures/gamepieces.h communication/sharedmem.c communication/sharedmem.h communication/sockets.c communication/sockets.h communication/buffered_communication.c communication/buffered_communication.h)

target_compile_options(LabProg PUBLIC -fsanitize=address)
target_link_libraries(LabProg m pthread rt -fsanitize=address)