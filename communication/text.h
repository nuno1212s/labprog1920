#ifndef LABPROG_TEXT_H
#define LABPROG_TEXT_H

#include "../structures/game.h"
#include "communications.h"

void txt_init(int host);

void txt_block();

int txt_readGameSize();

void txt_sendGameSize(int);

void txt_readPlayerInformation(Player *);

void txt_sendPlayerInformation(Player *);

void txt_writePossiblePieces(PossiblePieces *);

void txt_waitForOtherPlayerToChoosePieces();

PossiblePieces * txt_readPossiblePieces(Game *g);

void txt_sendAttemptedPlay(Position *, int, int);

Played txt_receiveAttemptedPlay(int);

HitResult txt_receivedAttemptedPlayResult(int gameID);

void txt_respondToAttemptedPlay( int playerID, HitType hit, int gameID);

void txt_destroy();

#endif //LABPROG_TEXT_H
