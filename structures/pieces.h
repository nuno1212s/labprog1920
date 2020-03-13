#ifndef LABPROG_PIECES_H
#define LABPROG_PIECES_H

#include "position.h"

struct Piece_ {

    int size;

    Position **positions;

    char *name;

};

struct PieceInBoard_ {

    struct Piece_ *piece;

    Position *basePos;

    PlacedDirection direction;

    int destroyed;

};

typedef struct Piece_ Piece;

typedef struct PieceInBoard_ PieceInBoard;

/**
 * Placed pieces
 * @return
 */

/**
 * Creates a new piece that is placed on the game board
 *
 * This method clones the position so the position that
 * is passed can be safely deleted.
 */
PieceInBoard *initPieceInBoard(Piece *, Position *, PlacedDirection);

Piece *gs_getPieceType(PieceInBoard *);

Position *gs_getBasePosition(PieceInBoard *);

PlacedDirection gs_getPlacedDir(PieceInBoard *);

void setDestroyed(PieceInBoard *);

int isDestroyed(PieceInBoard *);

void gs_freePIB(PieceInBoard *);

/**
 * Pieces
 */

/**
 * Creates a new piece object
 *
 * Takes the size, name and array of positions
 *
 * The name and array of positions are cloned and therefore can be called with
 * Local variables.
 */
Piece *initPiece(int, char *, Position **);

char *getName(Piece *);

Position **getPositions(Piece *);

void gs_freePiece(Piece *);

#endif