#ifndef LABPROG_GRAPHICS_H
#define LABPROG_GRAPHICS_H

#include "../structures/gamestructures.h"
#include "../structures/game.h"

typedef enum {
    SHELL
} GRAPHICS;

/**
 * Initialize the graphics to a certain graphics configuration
 */
void initGraphics(GRAPHICS);

/**
 * Display the menu of the game
 * @return The game object of the game
 */
Game* displayMenu();

void displayGame(Game *);

#endif //LABPROG_GRAPHICS_H
