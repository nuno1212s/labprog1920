#ifndef LABPROG_GAMESTRUCTURES_H
#define LABPROG_GAMESTRUCTURES_H

#include "position.h"
#include "pieces.h"
#include "../storagestructures/linkedlist.h"

typedef enum {
    GS_MATRIX,
    GS_QUAD
} GameStorageType;

struct PointStorage_ {

    struct PieceInBoard_ *piece;

    struct HitPoint_ *opponentHitPoint, *ownHitPoint;

};

struct HitPoint_ {
    int hit;
};

struct GameStorage_ {

    GameStorageType type;

    int size;

    void *data;
};

struct HitResponse_ {

    enum {
        HR_HIT_BOAT,
        HR_HIT_NOTHING,
        HR_ALREADY_HIT
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
HitPoint *initHitPoint(int);

int gs_hasHit(HitPoint *);

void gs_freeHP(HitPoint *);

/**
 * GameStorages
 */
GameStorage *initGameStorage(int size);

void freeGameStorage(GameStorage *);

/**
 * Actual game methods
 */

/**
 * Attempt to insert the piece into the game's storage
 *
 * The position is not copied.
 *
 * @return The piece in the board object if the piece was successfully inserted, NULL if it wasn't inserted.
 */
PieceInBoard* insertPiece(GameStorage *, Piece*, Position *, PlacedDirection);

/**
 * Check if the player can place the piece in the given position
 * @return
 */
int gs_canPlayPiece(GameStorage *, Piece *, Position *, PlacedDirection);

/**
 * Remove a placed piece from the player's game
 */
void removePlacedPiece(GameStorage *, PieceInBoard *);

/**
 * Attempt to play a hit by the adversary
 *
 * @return The response of the hit
 */
HitResponse attemptHit(GameStorage *, Position *);

/**
 * Get the point data stored in the given position
 * @return
 */
PointStorage *getAtPosition(GameStorage *, Position *);

/**
 * Check if we have played in a specific position
 * @return
 */
int hasPlayed(GameStorage *, Position *);

/**
 * Register our own play
 *
 * @param result
 */
void registerHit(GameStorage *, Position *, int result);

/**
 * Check if a piece has already been destroyed
 * @return
 */
int hasBeenDestroyed(GameStorage *, PieceInBoard *);

/**
 * Calculate the new position that a piece will occupy when placed in the position base with the direction dir.
 *
 * @param piece The piece to place
 * @param base The location to place it in
 * @param dir The direction to place the ball in
 * @return
 */
Position **calculateNewPositions(Piece *piece, Position *base, PlacedDirection dir);

void iterateAllStoredPoints(GameStorage *, void (*)(PointStorage *));

#endif //LABPROG_GAMESTRUCTURES_H
