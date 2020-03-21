#ifndef LABPROG_GRAPHICS_H
#define LABPROG_GRAPHICS_H

#include "../structures/gamestructures.h"
#include "../structures/game.h"

typedef enum {
    SHELL
} GRAPHICS;

void initGraphics(GRAPHICS);

void displayMenu();

void displayGame();

#endif //LABPROG_GRAPHICS_H
