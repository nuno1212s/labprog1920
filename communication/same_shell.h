#ifndef LABPROG_SAME_SHELL_H
#define LABPROG_SAME_SHELL_H

#include "communications.h"

void ssh_init();

int ssh_readGameSize();

void ssh_writeGameSize(int);

void ssh_sendGameInfo(Game *);

void ssh_readPlayerInformation(Player *);

void ssh_sendPlayerInformation(Player *);

void ssh_sendPossiblePieces(PossiblePieces *);

PossiblePieces *ssh_receivePossiblePieces(Game *game);

void ssh_waitForOtherPlayerToChoosePieces();

void ssh_sendAttemptedPlay(Position *pos, int playerID, int gameID);

Played ssh_receiveAttemptedPlay(int gameID);

void ssh_respondToAttemptedPlay(int playerID, HitType hit, int gameID);

HitResult ssh_receivedAttemptedPlayResult(int gameID);

#endif //LABPROG_SAME_SHELL_H
