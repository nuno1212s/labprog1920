#include "pieces.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

PieceInBoard *initPieceInBoard(Piece *piece, Position *pos, PlacedDirection dir) {

    PieceInBoard *board = malloc(sizeof(PieceInBoard));

    board->piece = piece;
    board->direction = dir;
    board->basePos = clonePos(pos);
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

Piece *initPiece(int size, char *name, BitMatrix *positions) {

    Piece *piece = malloc(sizeof(Piece));

    piece->name = strdup(name);
    piece->size = size;
    piece->matrix = positions;

    return piece;
}

BitMatrix *getPositions(Piece *piece) {
    return piece->matrix;
}

char *getName(Piece *piece) {
    return piece->name;
}

void gs_freePiece(Piece *piece) {

    printf("Freeing piece %s\n", piece->name);

    free(piece->name);

    freeBitMatrix(piece->matrix);

    free(piece);
}
