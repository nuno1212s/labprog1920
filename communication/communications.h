#ifndef LABPROG_COMMUNICATIONS_H
#define LABPROG_COMMUNICATIONS_H

#include "../structures/game.h"

typedef struct {

    Position *pos;
    int playerID;
    int gameID;

} Played;

typedef struct {

    int playerID;

    HitType type;

} HitResult;

/**
 * Initialize the comms
 *
 * @param host Whether this process is the parent process
 */
void initComms(int host);

void c_block();

int c_readGameSize();

void c_writeGameSize(int);

void c_readPlayerInformation(int id, Player *);

void c_sendPlayerInformation(int id, Player *);

void c_sendPossiblePieces(PossiblePieces *);

PossiblePieces *c_receivePossiblePieces(Game *game);

void c_waitForOtherPlayerToChoosePieces();

void c_sendGameInfo(Game *);

void c_readGameInfo(Game *);

void c_sendAttemptedPlay(Position *pos, int playerID, int gameID);

Played c_receiveAttemptedPlay(int gameID);

void c_respondToAttemptedPlay(int playerID, HitType hit, int gameID);

HitResult c_receivedAttemptedPlayResult(int gameID);

void c_destroy();

#endif //LABPROG_COMMUNICATIONS_H
