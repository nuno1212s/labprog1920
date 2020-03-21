#include "pieces.h"
#include <stdlib.h>
#include <string.h>

PieceInBoard *initPieceInBoard(Piece *piece, Position *pos, PlacedDirection dir) {

    PieceInBoard *board = malloc(sizeof(PieceInBoard));

    board->piece = piece;
    board->direction = dir;
    board->basePos = pos;
    board->destroyed = 0;

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

void incrementDestroyed(PieceInBoard *piece) {
    piece->destroyed++;
}

int isDestroyed(PieceInBoard *piece) {
    return piece->destroyed == piece->piece->size;
}

void gs_freePIB(PieceInBoard *piece) {

    p_free(piece->basePos);

    free(piece);

}

Piece *initPiece(int size, char *name, Position **positions) {

    Piece *piece = malloc(sizeof(Piece));

    piece->name = strdup(name);
    piece->size = size;
    piece->positions = malloc(sizeof(Position *) * size);

    for (int i = 0; i < size; i++) {

        piece->positions[i] = clonePos(positions[i]);

    }

    return piece;

}

void freePiece(Piece *piece) {

    free(piece->name);

    for (int i = 0; i < piece->size; i++) {
        p_free(piece->positions[i]);
    }


    free(piece->positions);

    free(piece);
}
