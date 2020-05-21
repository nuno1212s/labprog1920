#ifndef LABPROG_SHELL2_0_H
#define LABPROG_SHELL2_0_H

#include "../structures/game.h"

int sh_readGameSize();

char *sh_readPlayerName();

Position *sh_readPosition();

void sh_alreadyPlayedThere();

void sh_missed();

void sh_hitBoat();

void sh_destroyedBoat();

void sh_showPlaceablePieces(Player *, PossiblePieces *, int *placed);

void sh_showNotPossibleToPlace(Piece *, Position *);

void sh_showPiecePlaced(Piece *, Position *);

void sh_showAllPlaced();

int sh_requestPieceToPlay();

PlacedDirection sh_readPlaceDirection();

#endif //LABPROG_SHELL2_0_H
