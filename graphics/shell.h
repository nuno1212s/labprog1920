#ifndef LABPROG_SHELL_H
#define LABPROG_SHELL_H

#include "../structures/game.h"
#include "../gameplay.h"

PossiblePieces *sh_readPossiblePieces(Game *);

int sh_readGameSize();

char *sh_readPlayerName();

Position *sh_readPosition();

void sh_alreadyPlayedThere();

void sh_displayGameSize(int);

void sh_displayOpponentName(char *);

void sh_missed();

void sh_hitBoat();

void sh_destroyedBoat();

void sh_showPlaceablePieces(Player *, PossiblePieces *, PieceInBoard **placed);

void sh_showNotPossibleToPlace(Piece *, Position *);

void sh_showPiecePlaced(Piece *, Position *);

void sh_waitingForOpponent();

void sh_showAllPlaced();

int sh_requestPieceToPlay(int maxSize);

PlacedDirection sh_readPlaceDirection();

void sh_showYourTurn(Player *player);

void sh_showOtherTurn(Player *player);

#endif //LABPROG_SHELL_H
