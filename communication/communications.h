#ifndef LABPROG_COMMUNICATIONS_H
#define LABPROG_COMMUNICATIONS_H

#include "../structures/game.h"

typedef enum {

    SAME_SHELL,
    TEXT,
    PIPES,
    NETWORK

} COMM_TYPE;

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
void initComms(COMM_TYPE, int host);

int c_readGameSize();

void c_writeGameSize(int);

void c_readPlayerInformation(Player *);

void c_sendPlayerInformation(Player *);

void c_sendPossiblePieces(PossiblePieces *);

PossiblePieces *c_receivePossiblePieces(Game *game);

void c_waitForOtherPlayerToChoosePieces();

void c_sendAttemptedPlay(Position *pos, int playerID, int gameID);

Played c_receiveAttemptedPlay(int gameID);

void c_respondToAttemptedPlay(int playerID, HitType hit, int gameID);

HitResult c_receivedAttemptedPlayResult(int gameID);

#endif //LABPROG_COMMUNICATIONS_H
