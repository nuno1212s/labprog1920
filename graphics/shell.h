
#ifndef LABPROG_SHELL_H
#define LABPROG_SHELL_H

#include "../structures/game.h"

Game *sg_displayMenu();

void sg_displayGame(Game *);

void displayGameTray(Player *, int size);

/**
 * User defined possible game pieces
 * @return
 */
PossiblePieces *readPossiblePieces(int traySize);

#endif //LABPROG_SHELL_H
