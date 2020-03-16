#ifndef LABPROG_GAMESTRUCTURES_H
#define LABPROG_GAMESTRUCTURES_H

#include "position.h"
#include "../storagestructures/quadtree.h"
#include "../storagestructures/matrix.h"
#include "pieces.h"
#include "../storagestructures/linkedlist.h"

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

struct HitResponse_ {

    enum {
        HIT_BOAT,
        HIT_NOTHING,
        ALREADY_HIT
    } hit_type;

    PieceInBoard *hit;

};

typedef struct HitPoint_ HitPoint;

typedef struct PointStorage_ PointStorage;

typedef struct GameStorage_ GameStorage;

typedef struct HitResponse_ HitResponse;

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
 * @return The piece in the board object if the piece was successfully inserted, NULL if it wasn't inserted.
 */
PieceInBoard* insertPiece(GameStorage *, Piece*, Position *, PlacedDirection);

int gs_canPlayPiece(GameStorage *, Piece *, Position *, PlacedDirection);

/**
 * Attempt to play a hit
 *
 * @return The response of the hit
 */
HitResponse attemptHit(GameStorage *, Position *);

/**
 * Check if th
 * @return
 */
int hasBeenDestroyed(GameStorage *, PieceInBoard *);

void iterateAllStoredPoints(GameStorage *, void (*)(PointStorage *));

#endif //LABPROG_GAMESTRUCTURES_H
