#ifndef LABPROG_GRAPHICS_H
#define LABPROG_GRAPHICS_H

#include "../structures/gamestructures.h"
#include "../structures/game.h"

typedef enum {
    SHELL
} GRAPHICS;

void initGraphics(GRAPHICS);

Game* displayMenu();

void displayGame(Game *);

#endif //LABPROG_GRAPHICS_H
