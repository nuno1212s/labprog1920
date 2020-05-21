#include "communications.h"
#include "text.h"
#include "sem.h"

static COMM_TYPE type;

void initComms(COMM_TYPE commType, int host) {
    type = commType;

    switch (type) {
        case TEXT:
            txt_init(host);
            break;
    }
}

int c_readGameSize() {

    switch (type) {
        case TEXT:
            return txt_readGameSize();
    }

}

void c_writeGameSize(int size) {
    switch (type) {
        case TEXT:
            txt_sendGameSize(size);
            break;
    }

}

void c_sendPossiblePieces(PossiblePieces *pieces) {

    switch (type) {

        case TEXT:
            txt_writePossiblePieces(pieces);
            break;
    }

}

void c_readPlayerInformation(Player *player) {

    switch (type) {

        case TEXT:
            txt_readPlayerInformation(player);
            break;
    }

}

void c_sendPlayerInformation(Player *player) {

    switch (type) {

        case TEXT:
            txt_sendPlayerInformation(player);
            break;

    }

}

PossiblePieces *c_receivePossiblePieces(Game *game) {

    switch (type) {

        case TEXT:
         return txt_readPossiblePieces(game);

    }
}

void c_sendAttemptedPlay(Position *pos, int playerID, int gameID) {


    switch (type) {

        case TEXT:
            txt_sendAttemptedPlay(pos, playerID, gameID);
            break;
    }

}

Played c_receiveAttemptedPlay(int gameID) {
    switch (type) {

        case TEXT:
            return txt_receiveAttemptedPlay(gameID);

    }
}

HitResult c_receivedAttemptedPlayResult(int gameID) {

    switch (type) {
        case TEXT:
            return txt_receivedAttemptedPlayResult(gameID);
    }

}

void c_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {

    switch (type) {
        case TEXT:
            txt_respondToAttemptedPlay(playerID, hit, gameID);
            break;
    }

}