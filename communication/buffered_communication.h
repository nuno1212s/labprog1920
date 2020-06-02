#ifndef LABPROG_BUFFERED_COMMUNICATION_H
#define LABPROG_BUFFERED_COMMUNICATION_H

#include "communications.h"

void bf_init(int host);

void bf_block();

int bf_readGameSize();

void bf_writeGameSize(int);

void bf_readPlayerInformation(int id, Player *);

void bf_sendPlayerInformation(int id, Player *);

void bf_sendPossiblePieces(PossiblePieces *);

PossiblePieces *bf_receivePossiblePieces(Game *game);

void bf_waitForOtherPlayerToChoosePieces();

void bf_sendGameInfo(Game *);

void bf_readGameInfo(Game *);

void bf_sendAttemptedPlay(Position *pos, int playerID, int gameID);

Played bf_receiveAttemptedPlay(int gameID);

void bf_respondToAttemptedPlay(int playerID, HitType hit, int gameID);

HitResult bf_receivedAttemptedPlayResult(int gameID);

void bf_destroy();

#endif //LABPROG_BUFFERED_COMMUNICATION_H
