#include "gamestructures.h"
#include <stdlib.h>


HitPoint *initHitPoint(Position *pos, int isHit) {

    HitPoint * point = malloc(sizeof(HitPoint));

    point->pos = pos;

    point->hit = isHit;

    return point;
}

int gs_hasHit(HitPoint *point) {
    return point->hit;
}

Position *gs_getPos(HitPoint *point) {
    return point->pos;
}

void gs_freeHP(HitPoint *hp) {
    free(gs_getPos(hp));

    free(hp);
}

PieceInBoard *initPieceInBoard(Piece *piece, Position *pos, PlacedDirection dir) {

    PieceInBoard *board = malloc(sizeof(PieceInBoard));

    board->piece = piece;
    board->direction = dir;
    board->basePos = pos;

    return board;
}

Piece *gs_getPieceType(PieceInBoard *piece) {
    return piece->piece;
}

Position *gs_getBasePosition(PieceInBoard *piece) {
    return piece->basePos;
}

PlacedDirection gs_getPlacedDir(PieceInBoard *piece) {
    return piece->direction;
}