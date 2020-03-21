#ifndef LABPROG_COMMUNICATIONS_H
#define LABPROG_COMMUNICATIONS_H

#include "../structures/game.h"

void sendAttemptedPlay(Position *pos, int playerID, int gameID);

void receiveAttemptedPlayResult(Position *pos, int playerID, int hit, int gameID);

#endif //LABPROG_COMMUNICATIONS_H
