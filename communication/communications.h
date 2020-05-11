#ifndef LABPROG_COMMUNICATIONS_H
#define LABPROG_COMMUNICATIONS_H

#include "../structures/game.h"

typedef enum {

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

    Position *pos;

    int playerID;

    HitType type;

} HitResult;

void initComms(COMM_TYPE);

void sendPossiblePieces(PossiblePieces *);

PossiblePieces *receivePossiblePieces();

void sendAttemptedPlay(Position *pos, int playerID, int gameID);

Played receiveAttemptedPlay(int gameID);

HitResult receivedAttemptedPlayResult(int gameID);

void respondToAttemptedPlay(Position *pos, int playerID, int hit, int gameID);

#endif //LABPROG_COMMUNICATIONS_H
