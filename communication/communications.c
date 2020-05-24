
#include "communications.h"

#define TEXT

#ifdef TEXT

#include "text.h"

#else

#include "same_shell.h"

#endif

void initComms(int host) {
#ifdef TEXT
    txt_init(host);
#else

#endif
}

int c_readGameSize() {

#ifdef TEXT
    return txt_readGameSize();
#endif
}

void c_block() {

#ifdef TEXT
    txt_block();
#endif

}

void c_writeGameSize(int size) {

#ifdef TEXT
    txt_sendGameSize(size);
#endif

}

void c_sendPossiblePieces(PossiblePieces *pieces) {

#ifdef TEXT
    txt_writePossiblePieces(pieces);
#endif

}

void c_readPlayerInformation(int id, Player *player) {

#ifdef TEXT
    txt_readPlayerInformation(id, player);
#endif
}

void c_sendPlayerInformation(int id, Player *player) {

#ifdef TEXT
    txt_sendPlayerInformation(id, player);
#endif

}

PossiblePieces *c_receivePossiblePieces(Game *game) {
#ifdef TEXT
    return txt_readPossiblePieces(game);
#endif
}

void c_sendAttemptedPlay(Position *pos, int playerID, int gameID) {
#ifdef TEXT
    txt_sendAttemptedPlay(pos, playerID, gameID);
#endif
}

void c_sendGameInfo(Game *game) {
#ifdef TEXT
    txt_sendGameInfo(game);
#endif

}

void c_readGameInfo(Game *game) {

#ifdef TEXT
    txt_readGameInfo(game);
#endif

}

Played c_receiveAttemptedPlay(int gameID) {

#ifdef TEXT
    return txt_receiveAttemptedPlay(gameID);
#endif
}

HitResult c_receivedAttemptedPlayResult(int gameID) {

#ifdef TEXT
    return txt_receivedAttemptedPlayResult(gameID);
#endif

}

void c_waitForOtherPlayerToChoosePieces() {

#ifdef TEXT
    txt_waitForOtherPlayerToChoosePieces();
#endif

}

void c_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {

#ifdef TEXT
    txt_respondToAttemptedPlay(playerID, hit, gameID);
#endif
}

void c_destroy() {
#ifdef TEXT
    txt_destroy();
#endif
}