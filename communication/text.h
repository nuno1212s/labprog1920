#ifndef LABPROG_TEXT_H
#define LABPROG_TEXT_H

#include "../structures/game.h"
#include "communications.h"

void txt_init(int host);

int txt_readGameSize();

void txt_sendGameSize(int);

void txt_readPlayerInformation(Player *);

void txt_sendPlayerInformation(Player *);

void txt_writePossiblePieces(PossiblePieces *);

PossiblePieces * txt_readPossiblePieces(Game *g);

void txt_sendAttemptedPlay(Position *, int, int);

Played txt_receiveAttemptedPlay(int);

HitResult txt_receivedAttemptedPlayResult(int gameID);

void txt_respondToAttemptedPlay( int playerID, HitType hit, int gameID);

#endif //LABPROG_TEXT_H
