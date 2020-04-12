
#ifndef LABPROG_SHELL_H
#define LABPROG_SHELL_H

#include "../structures/game.h"

Game* sg_displayMenu();

void displayGameTray(Player *, int size);

/**
 * User defined possible game pieces
 * @return
 */
PossiblePieces*readPossiblePieces(int traySize);

Piece* readPiece();

#endif //LABPROG_SHELL_H
