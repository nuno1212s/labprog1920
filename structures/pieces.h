#ifndef LABPROG_PIECES_H
#define LABPROG_PIECES_H

#include "position.h"
#include "../storagestructures/bitmap.h"

#define PIECE_MATRIX_WORD_SIZE 1
#define PIECE_MATRIX_ROWS 5
#define PIECE_MATRIX_COLS 5

struct Piece_ {

    int size;

    BitMatrix *matrix;

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

/**
 * Get the piece structure that supports this placed piece
 * @return
 */
Piece *gs_getPieceType(PieceInBoard *);

Position *gs_getBasePosition(PieceInBoard *);

/**
 * Get the direction that the placed piece is in
 * @return
 */
PlacedDirection gs_getPlacedDir(PieceInBoard *);

/**
 * Increment the amount of spots that have been destroyed in this piece
 */
void incrementDestroyed(PieceInBoard *);

/**
 * Check if the piece is destroyed
 * @return
 */
int isDestroyed(PieceInBoard *);

/**
 * Free a piece that has been placed
 *
 * This does not free the underlying piece obj
 */
void gs_freePIB(PieceInBoard *);

/**
 * Pieces
 */

/**
 * Creates a new piece object
 *
 * Takes the size, name and array of positions
 *
 * The name is cloned and therefore can be called with
 * Local variables.
 */
Piece *initPiece(int, char *, BitMatrix *matrix);

/**
 * Get the name of a piece
 * @return
 */
char *getName(Piece *);

/**
 * Get the positions associated with a piece
 * @return
 */
BitMatrix *getPositions(Piece *);

/**
 * Free the piece object
 */
void gs_freePiece(Piece *);

#endif