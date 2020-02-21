#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

PlayedGamePiece *initPlayedPiece(GamePiece *gamePiece, Position *position) {

    PlayedGamePiece *piece = malloc(sizeof(PlayedGamePiece));

    piece->pieceType = OWN_PIECE;

    piece->position = position;

    piece->piece = gamePiece;

    return piece;
}

PlayedGamePiece *initAttemptPiece(int hit, Position *position) {

    PlayedGamePiece *piece = malloc(sizeof(PlayedGamePiece));

    piece->pieceType = PLAYED_PIECE;

    piece->hit = hit;

    piece->position = position;

    return piece;
}

GamePiece * g_getGamePiece(PlayedGamePiece *p) {

    if (p->pieceType == OWN_PIECE) {
        return p->piece;
    } else {
        fprintf(stderr, "Attempted a g_getGamePiece on a piece that is not your own.");
        exit(1);
    }
}

Position *g_getPlayedPosition(PlayedGamePiece *p) {
    return p->position;
}

int g_isHit(PlayedGamePiece *p) {

    if (p->pieceType == PLAYED_PIECE) {
        return p->hit;
    } else {
        fprintf(stderr, "Attempted a g_isHit on a piece that is your own.");
        exit(1);
    }

}

void g_freePlayed(PlayedGamePiece *piece) {

    p_free(piece->position);

    free(piece);

}