//
// Created by nuno on 10/05/20.
//

#ifndef LABPROG_TEXT_H
#define LABPROG_TEXT_H

#include "../structures/game.h"
#include "communications.h"

void txt_init();

void txt_writePossiblePieces(PossiblePieces *);

PossiblePieces * txt_readPossiblePieces();

void txt_sendAttemptedPlay(Position *, int, int);

Played txt_receiveAttemptedPlay(int);

HitResult txt_receivedAttemptedPlayResult(int gameID);

void txt_respondToAttemptedPlay(Position *pos, int playerID, int hit, int gameID);

#endif //LABPROG_TEXT_H
