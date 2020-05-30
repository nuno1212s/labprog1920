#ifndef LABPROG_PIPES_H
#define LABPROG_PIPES_H

#include "communications.h"

void p_init(int host);

void p_block();

int p_readGameSize();

void p_writeGameSize(int);

void p_readPlayerInformation(int id, Player *);

void p_sendPlayerInformation(int id, Player *);

void p_sendPossiblePieces(PossiblePieces *);

PossiblePieces *p_receivePossiblePieces(Game *game);

void p_waitForOtherPlayerToChoosePieces();

void p_sendGameInfo(Game *);

void p_readGameInfo(Game *);

void p_sendAttemptedPlay(Position *pos, int playerID, int gameID);

Played p_receiveAttemptedPlay(int gameID);

void p_respondToAttemptedPlay(int playerID, HitType hit, int gameID);

HitResult p_receivedAttemptedPlayResult(int gameID);

void p_destroy();

#endif //LABPROG_PIPES_H
