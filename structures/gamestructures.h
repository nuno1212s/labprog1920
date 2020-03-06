#ifndef LABPROG_GAMESTRUCTURES_H
#define LABPROG_GAMESTRUCTURES_H

#include "position.h"
#include "../storagestructures/quadtree.h"
#include "../storagestructures/matrix.h"
#include "pieces.h"

typedef enum {
    GS_MATRIX,
    GS_QUAD
} GameStorageType;

struct PointStorage_ {

    struct PieceInBoard_ *piece;

    struct HitPoint_ *hitPoint;

};

struct HitPoint_ {

    Position *pos;

    int hit;

};

struct GameStorage_ {

    GameStorageType type;

    union {
        Matrix *matrix;

        QuadTree *quadTree;
    } data;
};

typedef struct HitPoint_ HitPoint;

typedef struct PointStorage_ PointStorage;

typedef struct GameStorage_ GameStorage;

/**
 * Hit points
 */
HitPoint *initHitPoint(Position *, int);

int gs_hasHit(HitPoint *);

Position *gs_getPos(HitPoint *);

void gs_freeHP(HitPoint *);

/**
 * GameStorages
 */
GameStorage *initGameStorage(int size, GameStorageType type);

void freeGameStorage(GameStorage *);

/**
 * Actual game methods
 */

/**
 * Attempt to insert the piece into the game's storage
 * @return != 0 if the piece was successfully inserted, 0 if not
 */
int insertPiece(GameStorage *, Piece*, Position *, PlacedDirection);

/**
 * Attempt to play a hit
 *
 * @return != 0 if it hit anything, 0 if not
 */
int attemptHit(GameStorage *, Position *);

#endif //LABPROG_GAMESTRUCTURES_H
