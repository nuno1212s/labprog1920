#ifndef LABPROG_SHELL2_0_H
#define LABPROG_SHELL2_0_H

#include "../structures/game.h"
#include "../gameplay.h"

PossiblePieces *sh_readPossiblePieces(Game *);

GAME_TYPE sh_readGameType();

int sh_readGameSize();

char *sh_readPlayerName();

Position *sh_readPosition();

void sh_alreadyPlayedThere();

void sh_missed();

void sh_hitBoat();

void sh_destroyedBoat();

void sh_showPlaceablePieces(Player *, PossiblePieces *, PieceInBoard **placed);

void sh_showNotPossibleToPlace(Piece *, Position *);

void sh_showPiecePlaced(Piece *, Position *);

void sh_showAllPlaced();

int sh_requestPieceToPlay(int maxSize);

PlacedDirection sh_readPlaceDirection();

void sh_showYourTurn(Player *player);

void sh_showOtherTurn(Player *player);

#endif //LABPROG_SHELL2_0_H
