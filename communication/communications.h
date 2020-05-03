#ifndef LABPROG_COMMUNICATIONS_H
#define LABPROG_COMMUNICATIONS_H

#include "../structures/game.h"

typedef struct {

    Position *pos;
    int playerID;

} Played;

typedef struct {

    Position *pos;

    int playerID;

    HitType type;

} HitResult;

void sendAttemptedPlay(Position *pos, int playerID, int gameID);

HitResult receivedAttemptedPlayResult(int gameID);

Played receiveAttemptedPlay(int gameID);

void respondToAttemptedPlay(Position *pos, int playerID, int hit, int gameID);

#endif //LABPROG_COMMUNICATIONS_H
