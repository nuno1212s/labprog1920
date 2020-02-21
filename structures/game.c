#include "game.h"
#include <stdlib.h>
#include <string.h>

GamePiece *initGamePiece(int size, char *name) {

    GamePiece *piece = malloc(sizeof(GamePiece));

    piece->name = strdup(name);

    piece->size = size;

    return piece;
}

int g_getPieceSize(GamePiece *p) {
    return p->size;
}

char *g_getPieceName(GamePiece *p) {
    return p->name;
}

void g_freePiece(GamePiece *p) {

    free(p->name);

    free(p);
}

PlayedGamePiece *initPlayedPiece(GamePiece *gamePiece, Player *player, Position *position) {

    PlayedGamePiece *piece = malloc(sizeof(PlayedGamePiece));

    piece->position = position;

    piece->piece = gamePiece;

    piece->owner = player;

    return piece;
}

GamePiece * g_getGamePiece(PlayedGamePiece *p) {
    return p->piece;
}

Position *g_getPlayedPosition(PlayedGamePiece *p) {
    return p->position;
}

void g_freePlayed(PlayedGamePiece *piece) {

    p_free(piece->position);

    free(piece);

}