#include <stdio.h>

#include "graphics/graphics.h"

#include "storagestructures/bitmap.h"

#include "communication/communications.h"

int main() {
//    printf("Hello, World!\n");
//
//    Game* currentGame = displayMenu();
//
//    freeGame(currentGame);
//
    initComms(TEXT);

    printf("COMS\n");

    Position pos;

    pos.x = 10;
    pos.y = 20;

    sendAttemptedPlay(&pos, 1, 1);

    printf("SENT\n");

    Played played = receiveAttemptedPlay(1);

    printf("RECEIVED\n");

    printf("POS: %d %d PLAYERID: %d\n", p_getBaseX(played.pos), p_getBaseY(played.pos), played.playerID);
    return 0;
}
