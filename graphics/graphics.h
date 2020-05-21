#ifndef LABPROG_GRAPHICS_H
#define LABPROG_GRAPHICS_H

#include "../structures/gamestructures.h"
#include "../structures/game.h"
#include "../gameplay.h"

typedef enum {
    SHELL
} GRAPHICS;

/**
 * Initialize the graphics to a certain graphics configuration
 */
void initGraphics(GRAPHICS);

GAME_TYPE g_readGameType();

PossiblePieces* g_readPossiblePieces();

int g_readGameSize();

char *g_readPlayerName();

Position *g_readPosition();

void g_alreadyPlayedThere();

void g_missed();

void g_hitBoat();

void g_destroyedBoat();

void g_showPlaceablePieces(Player *, PossiblePieces *, int *placed);

void g_showNotPossibleToPlace(Piece *, Position *);

void g_showPiecePlaced(Piece *, Position *);

void g_showAllPlaced();

int g_requestPieceToPlay();

PlacedDirection g_readPlaceDirection();

#endif //LABPROG_GRAPHICS_H
