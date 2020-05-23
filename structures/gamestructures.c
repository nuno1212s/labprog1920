#include "gamestructures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Generic storage access function declarations

static void *initStorage(int size);

static void *lookUp(void *, Position *);

static void insert(void *, Position *, void *);

static void *delete(void *, Position *);

static void iterateAllPoints(void *, void (*)(PointStorage *));

static void freeStorage(void *);

#define QUAD

#ifdef QUAD

#include "../storagestructures/quadtree.h"

void *initStorage(int size) {

    Position topLeft = {0, size}, bottomRight = {size, 0};

    return initQuadTree(&topLeft, &bottomRight);

}

void *lookUp(void *storage, Position *pos) {
    return qt_lookup((QuadTree*) storage, pos);
}

void insert(void *storage, Position *pos, void *toInsert) {
    return qt_insert((QuadTree *) storage, pos, toInsert);
}

void *delete(void *storage, Position *pos) {
    return qt_delete((QuadTree*) storage, pos);
}

void iterateAllPoints(void *storage, void (*toCall)(PointStorage *)) {
    q_iterateAllPoints((QuadTree *) storage, (void (*) (void*)) toCall);
}

void freeStorage(void *storage) {
    freeQuad((QuadTree*) storage);
}

#else

#include "../storagestructures/matrix.h"

void *lookUp(void *storage, Position *pos) {
    return m_lookup((Matrix *) storage, pos);
}

void insert(void *storage, Position *pos, void *toInsert) {
    m_insert((Matrix *) storage, pos, toInsert);
}

void *delete(void *storage, Position *pos) {
    return m_delete((Matrix *) storage, pos);
}

void iterateAllPoints(void *storage, void (*toCall)(PointStorage *)) {
    m_iterateAllPoints((Matrix *) storage, (void (*)(void *)) toCall);
}

void freeStorage(void *storage) {
    freeMatrix((Matrix *) storage);
}

#endif

PointStorage *initPS(HitPoint *point, PieceInBoard *board, HitPoint *ownHitPoint) {

    PointStorage *ps = malloc(sizeof(PointStorage));

    ps->piece = board;
    ps->opponentHitPoint = point;
    ps->ownHitPoint = ownHitPoint;

    return ps;
}

void freePS(PointStorage *ps) {

    if (ps->opponentHitPoint != NULL) {
        gs_freeHP(ps->opponentHitPoint);
    }

    free(ps);

}

HitPoint *initHitPoint(Position *pos, int isHit) {

    HitPoint *point = malloc(sizeof(HitPoint));

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
    p_free(gs_getPos(hp));

    free(hp);
}

GameStorage *initGameStorage(int size) {

    GameStorage *gs = malloc(sizeof(GameStorage));

    gs->size = size;

#ifdef QUAD
    gs->type = GS_QUAD;
#else
    gs->type = GS_MATRIX;
#endif

    gs->data = initStorage(size);

    return gs;
}

void freeGameStorage(GameStorage *gs) {

    //Release all the stored points
    iterateAllStoredPoints(gs, &freePS);

    freeStorage(gs->data);

    free(gs);
}

Position **calculateNewPositions(Piece *piece, Position *base, PlacedDirection dir) {

    Position **newPositions = malloc(sizeof(Position *) * piece->size);

    int pos = 0;

    for (int x = 0; x < matrix_cols(piece->matrix); x++) {

        for (int y = 0; y < matrix_rows(piece->matrix); y++) {

            if (m_getBit(piece->matrix, x, y)) {

                newPositions[pos++] = addToWithDirection(initPos(x, y), base, dir);

                if (pos >= piece->size) {
                    break;
                }
            }
        }
    }

    return newPositions;
}

static void freePositions(Position **pos, int size) {

    for (int i = 0; i < size; i++) {
        p_free(pos[i]);
    }

    free(pos);
}

Position **canPlace1BlockClearance(Piece *piece, Position *basePos, PlacedDirection dir, void *storage, int traySize) {

    Position **newPos = calculateNewPositions(piece, basePos, dir);

    for (int i = 0; i < piece->size; i++) {

        Position *pos = newPos[i];

        int oGX = p_getBaseX(pos), oGY = p_getBaseY(pos);

        if (oGX < 0 || oGX >= traySize || oGY < 0 || oGY >= traySize) {
            freePositions(newPos, piece->size);
            return NULL;
        }

        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {

                Position p = {p_getBaseX(pos) + x, p_getBaseY(pos) + y};

                PointStorage *canPlace = (PointStorage *) lookUp(storage, &p);

                if (canPlace != NULL && canPlace->piece != NULL) {

                    freePositions(newPos, piece->size);

                    return NULL;
                }

            }
        }
    }

    return newPos;
}

int gs_canPlayPiece(GameStorage *storage, Piece *piece, Position *pos, PlacedDirection dir) {

    void *storageObj = storage->data;

    Position **positions = canPlace1BlockClearance(piece, pos, dir, storageObj, storage->size);

    if (positions != NULL) {
        freePositions(positions, piece->size);

        return 1;
    } else {
        return 0;
    }
}

PieceInBoard *insertPiece(GameStorage *gs, Piece *piece, Position *basePos, PlacedDirection dir) {

    Position **positions;

    if ((positions = canPlace1BlockClearance(piece, basePos, dir, gs->data, gs->size)) != NULL) {
        PieceInBoard *board = initPieceInBoard(piece, basePos, dir);

        for (int i = 0; i < piece->size; i++) {

            PointStorage *ps = initPS(NULL, board, NULL);

            insert(gs->data, positions[i], ps);
        }

        freePositions(positions, piece->size);

        return board;

    }

    return NULL;
}

void removePlacedPiece(GameStorage *gs, PieceInBoard *board) {

    Position **pos = calculateNewPositions(board->piece, board->basePos, board->direction);

    for (int i = 0; i < board->piece->size; i++) {

        void *result = lookUp(gs->data, pos[i]);

        if (result == NULL) {
            continue;
        }

        PointStorage *p = (PointStorage *) result;

        freePS(p);

        delete(gs->data, pos[i]);
    }
}

HitResponse hitNothing() {
    HitResponse s;

    s.hit_type = HR_HIT_NOTHING;

    return s;
}

HitResponse hitBoat(PieceInBoard *hit) {
    HitResponse s;

    s.hit_type = HR_HIT_BOAT;
    s.hit = hit;

    return s;
}

HitResponse alreadyHit() {
    HitResponse s;

    s.hit_type = HR_ALREADY_HIT;

    return s;
}

HitResponse attemptHitStorage(void *storage, Position *toHit) {
    PointStorage *point = lookUp(storage, toHit);

    if (point == NULL) {

        insert(storage, toHit, initPS(initHitPoint(toHit, 0), NULL, NULL));

        return hitNothing();
    }

    if (point->opponentHitPoint != NULL) {
        return alreadyHit();
    }

    int isHit = 0;

    if (point->piece != NULL) {
        isHit = 1;
    }

    point->opponentHitPoint = initHitPoint(toHit, isHit);

    return hitBoat(point->piece);

}

PointStorage *getAtPosition(GameStorage *storage, Position *pos) {

    return (PointStorage *) lookUp(storage->data, pos);

}

HitResponse attemptHit(GameStorage *storage, Position *toHit) {

    HitResponse s = attemptHitStorage(storage->data, toHit);

    if (s.hit_type == HR_HIT_BOAT) {
        incrementDestroyed(s.hit);
    }

    return s;
}

void registerHit(GameStorage *storage, Position *pos, int result) {

    void *storageObj = storage->data;

    void *position = lookUp(storageObj, pos);

    HitPoint *ourHitPoint = initHitPoint(pos, result);

    if (position == NULL) {
        position = initPS(NULL, NULL, ourHitPoint);

        insert(storageObj, pos, position);
    } else {

        PointStorage *ps = (PointStorage *) position;

        ps->ownHitPoint = ourHitPoint;
    }

}

static int hasPlayedGeneric(void *storage, Position *pos) {

    void *result = lookUp(storage, pos);

    if (result == NULL) {
        return 0;
    } else if (((PointStorage *) result)->ownHitPoint == NULL) {
        return 0;
    }

    return 1;
}

int hasPlayed(GameStorage *storage, Position *pos) {
    return hasPlayedGeneric(storage->data, pos);
}

int hasBeenDestroyed(GameStorage *storage, PieceInBoard *board) {
    return isDestroyed(board);
}

void iterateAllStoredPoints(GameStorage *storage, void (*toCall)(PointStorage *)) {
    iterateAllPoints(storage->data, toCall);
}