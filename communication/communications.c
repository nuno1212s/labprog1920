#include "communications.h"
#include "text.h"

static COMM_TYPE type;

void initComms(COMM_TYPE commType) {
    type = commType;

    switch (type) {
        case TEXT:
            txt_init();
            break;
    }
}

void sendPossiblePieces(PossiblePieces *pieces) {

    switch (type) {

        case TEXT:
            txt_writePossiblePieces(pieces);
            break;
    }

}

PossiblePieces *receivePossiblePieces() {

    switch (type) {

        case TEXT:
         return txt_readPossiblePieces();

    }

}

void sendAttemptedPlay(Position *pos, int playerID, int gameID) {

    switch (type) {

        case TEXT:
            txt_sendAttemptedPlay(pos, playerID, gameID);
            break;
    }

}

Played receiveAttemptedPlay(int gameID) {
    switch (type) {

        case TEXT:
            return txt_receiveAttemptedPlay(gameID);

    }
}

HitResult receivedAttemptedPlayResult(int gameID) {

    switch (type) {
        case TEXT:
            return txt_receivedAttemptedPlayResult(gameID);
    }

}

void respondToAttemptedPlay(Position *pos, int playerID, int hit, int gameID) {

    switch (type) {
        case TEXT:
            txt_respondToAttemptedPlay(pos, playerID, hit, gameID);
    }

}