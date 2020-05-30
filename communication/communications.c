#include "communications.h"

#define SAME_SHELL

#ifdef TEXT

#include "text.h"

#elif defined(SAME_SHELL)

#include "same_shell.h"

#endif

void initComms(int host) {
#ifdef TEXT
    txt_init(host);
#elif defined(SAME_SHELL)

    ssh_init();

#endif
}

int c_readGameSize() {

#ifdef TEXT
    return txt_readGameSize();
#elif defined(SAME_SHELL)
    return ssh_readGameSize();
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
#elif defined(SAME_SHELL)
    ssh_writeGameSize(size);
#endif

}

void c_sendPossiblePieces(PossiblePieces *pieces) {

#ifdef TEXT
    txt_writePossiblePieces(pieces);
#elif defined(SAME_SHELL)
    ssh_sendPossiblePieces(pieces);
#endif

}

void c_readPlayerInformation(int id, Player *player) {

#ifdef TEXT
    txt_readPlayerInformation(id, player);
#elif defined(SAME_SHELL)
    ssh_readPlayerInformation(player);
#endif
}

void c_sendPlayerInformation(int id, Player *player) {

#ifdef TEXT
    txt_sendPlayerInformation(id, player);
#elif defined(SAME_SHELL)
    ssh_sendPlayerInformation(player);
#endif

}

PossiblePieces *c_receivePossiblePieces(Game *game) {
#ifdef TEXT
    return txt_readPossiblePieces(game);
#elif defined(SAME_SHELL)
    return ssh_receivePossiblePieces(game);
#endif
}

void c_sendAttemptedPlay(Position *pos, int playerID, int gameID) {
#ifdef TEXT
    txt_sendAttemptedPlay(pos, playerID, gameID);
#elif defined(SAME_SHELL)
    ssh_sendAttemptedPlay(pos, playerID, gameID);
#endif
}

void c_sendGameInfo(Game *game) {
#ifdef TEXT
    txt_sendGameInfo(game);
#elif defined(SAME_SHELL)
    ssh_sendGameInfo(game);
#endif

}

void c_readGameInfo(Game *game) {

#ifdef TEXT
    txt_readGameInfo(game);
#elif defined(SAME_SHELL)

#endif

}

Played c_receiveAttemptedPlay(int gameID) {

#ifdef TEXT
    return txt_receiveAttemptedPlay(gameID);
#elif defined(SAME_SHELL)
    return ssh_receiveAttemptedPlay(gameID);
#endif
}

HitResult c_receivedAttemptedPlayResult(int gameID) {

#ifdef TEXT
    return txt_receivedAttemptedPlayResult(gameID);
#elif defined(SAME_SHELL)
    return ssh_receivedAttemptedPlayResult(gameID);
#endif

}

void c_waitForOtherPlayerToChoosePieces() {

#ifdef TEXT
    txt_waitForOtherPlayerToChoosePieces();
#elif defined(SAME_SHELL)
    ssh_waitForOtherPlayerToChoosePieces();
#endif

}

void c_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {

#ifdef TEXT
    txt_respondToAttemptedPlay(playerID, hit, gameID);
#elif defined(SAME_SHELL)
    ssh_respondToAttemptedPlay(playerID, hit, gameID);
#endif
}

void c_destroy() {
#ifdef TEXT
    txt_destroy();
#endif
}