#include "gamestructures.h"
#include <stdlib.h>
#include <string.h>

PointStorage *initPS(HitPoint *point, PieceInBoard *board) {

    PointStorage *ps = malloc(sizeof(PointStorage));

    ps->piece = board;
    ps->hitPoint = point;

    return ps;
}

void freePS(PointStorage *ps) {

    if (ps->hitPoint != NULL) {
        gs_freeHP(ps->hitPoint);
    }

    if (ps->piece != NULL) {
        gs_freePIB(ps->piece);
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

GameStorage *initGameStorage(int size, GameStorageType type) {

    GameStorage *gs = malloc(sizeof(GameStorage));

    gs->type = type;

    switch (type) {

        case GS_MATRIX: {
            gs->data.matrix = initMatrix(size);
            break;
        }

        case GS_QUAD: {

            Position topLeft = {0, size},
                    bottomRight = {size, 0};

            gs->data.quadTree = initQuadTree(&topLeft, &bottomRight);
            break;
        }
    }

    return gs;
}

void freeGameStorage(GameStorage *gs) {

    //Release all the stored points
    iterateAllStoredPoints(gs, &freePS);

    switch (gs->type) {

        case GS_QUAD: {

            freeQuad(gs->data.quadTree);

            break;
        }

        case GS_MATRIX: {
            freeMatrix(gs->data.matrix);

            break;
        }

    }

    free(gs);
}

Position **calculateNewPositions(Piece *piece, Position *base, PlacedDirection dir) {

    Position **newPositions = malloc(sizeof(Position *) * piece->size);

    for (int i = 0; i < piece->size; i++) {

        newPositions[i] = addToWithDirection(clonePos(base), piece->positions[i], dir);

    }

    return newPositions;
}

void freePositions(Position **pos, int size) {

    for (int i = 0; i < size; i++) {
        p_free(pos[i]);
    }

    free(pos);
}

Position **canPlace1BlockClearance(Piece *piece, Position *basePos, PlacedDirection dir, void *storage,
                                   void *(*toCall)(void *, Position *)) {

    Position **newPos = calculateNewPositions(piece, basePos, dir);

    for (int i = 0; piece->size; i++) {

        Position *pos = newPos[i];

        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {

                Position p = {p_getBaseX(pos) + x, p_getBaseY(pos) + y};

                PointStorage *canPlace = (PointStorage *) toCall(storage, &p);

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

    void *(*toLookFunc)(void *, Position *);

    void *storageObj;

    switch (storage->type) {

        case GS_MATRIX: {
            toLookFunc = (void *(*)(void *, Position *)) m_lookup;

            storageObj = storage->data.matrix;

            break;
        }

        case GS_QUAD: {
            toLookFunc = (void *(*) (void *, Position *)) qt_lookup;

            storageObj = storage->data.quadTree;

            break;
        }

        default:
            return 0;

    }

    Position **positions = canPlace1BlockClearance(piece, pos, dir, storageObj, toLookFunc);

    if (positions != NULL) {
        freePositions(positions, piece->size);

        return 1;
    } else {
        return 0;
    }
}

PieceInBoard *insertPieceQuad(QuadTree *qt, Piece *piece, Position *basePos, PlacedDirection dir) {

    void *(*toLookFunc)(void *, Position *) = (void *(*)(void *, Position *)) qt_lookup;

    Position **pos;

    if ((pos = canPlace1BlockClearance(piece, basePos, dir, qt, toLookFunc)) != NULL) {
        PieceInBoard *board = initPieceInBoard(piece, basePos, dir);

        for (int i = 0; i < piece->size; i++) {
            PointStorage *ps = initPS(NULL, board);

            qt_insert(qt, pos[i], ps);
        }

        freePositions(pos, piece->size);

        return board;
    }

    return NULL;
}

PieceInBoard *insertPieceMatrix(Matrix *matrix, Piece *piece, Position *basePos, PlacedDirection dir) {
    void *(*lookUpFunc)(void *, Position *) = (void *(*)(void *, Position *)) &m_lookup;

    Position **positions;

    if ((positions = canPlace1BlockClearance(piece, basePos, dir, matrix, lookUpFunc)) != NULL) {
        PieceInBoard *board = initPieceInBoard(piece, basePos, dir);

        for (int i = 0; i < piece->size; i++) {

            PointStorage *ps = initPS(NULL, board);

            m_insert(matrix, positions[i], ps);
        }

        freePositions(positions, piece->size);

        return board;

    }

    return NULL;
}

PieceInBoard *insertPiece(GameStorage *gs, Piece *piece, Position *basePos, PlacedDirection dir) {

    switch (gs->type) {

        case GS_MATRIX: {
            return insertPieceMatrix(gs->data.matrix, piece, basePos, dir);
        }

        case GS_QUAD: {
            return insertPieceQuad(gs->data.quadTree, piece, basePos, dir);
        }

    }

    return NULL;
}

HitResponse hitNothing() {
    HitResponse s;

    s.hit_type = HIT_NOTHING;

    return s;
}

HitResponse hitBoat(PieceInBoard *hit) {
    HitResponse s;

    s.hit_type = HIT_BOAT;
    s.hit = hit;

    return s;
}

HitResponse alreadyHit() {
    HitResponse s;

    s.hit_type = ALREADY_HIT;

    return s;
}

HitResponse attemptHitQuad(QuadTree *tree, Position *toHit) {

    PointStorage *point = qt_lookup(tree, toHit);

    if (point == NULL) {

        PointStorage *storage = initPS(initHitPoint(toHit, 0), NULL);

        qt_insert(tree, toHit, storage);

        return hitNothing();
    }

    if (point->hitPoint != NULL) {

        return alreadyHit();

    }

    int isHit = 0;

    if (point->piece != NULL) {
        isHit = 1;
    }

    point->hitPoint = initHitPoint(toHit, isHit);

    return hitBoat(point->piece);
}

HitResponse attemptHitMatrix(Matrix *matrix, Position *toHit) {

    PointStorage *point = m_lookup(matrix, toHit);

    if (point == NULL) {

        m_insert(matrix, toHit, initPS(initHitPoint(toHit, 0), NULL));

        return hitNothing();
    }

    if (point->hitPoint != NULL) {
        return alreadyHit();
    }

    int isHit = 0;

    if (point->piece != NULL) {
        isHit = 1;
    }

    point->hitPoint = initHitPoint(toHit, isHit);

    return hitBoat(point->piece);
}

HitResponse attemptHit(GameStorage *storage, Position *toHit) {

    HitResponse s = hitNothing();

    switch (storage->type) {
        case GS_MATRIX:
            s = attemptHitMatrix(storage->data.matrix, toHit);
            break;
        case GS_QUAD:
            s = attemptHitQuad(storage->data.quadTree, toHit);
            break;
    }

    if (s.hit_type == HIT_BOAT) {
        hasBeenDestroyed(storage, s.hit);
    }

    return s;
}

int hasBeenDestroyedQuad(QuadTree *quad, PieceInBoard *board) {

    Piece *piece = board->piece;

    for (int i = 0; i < piece->size; i++) {

        Position *p = piece->positions[i];

        Position cP = {p_getBaseX(p), p_getBaseY(p)};

        addToWithDirection(&cP, board->basePos, board->direction);

        PointStorage *storage = qt_lookup(quad, &cP);

        if (storage == NULL) {
            return 0;
        }

        if (storage->hitPoint == NULL) {
            return 0;
        }
    }

    return 1;

}

int hasBeenDestroyedMatrix(Matrix *matrix, PieceInBoard *board) {

    Piece *piece = board->piece;

    for (int i = 0; i < piece->size; i++) {

        Position *p = piece->positions[i];

        Position cP = {p_getBaseX(p), p_getBaseY(p)};

        addToWithDirection(&cP, board->basePos, board->direction);

        PointStorage *storage = m_lookup(matrix, &cP);

        if (storage == NULL) {
            return 0;
        }

        if (storage->hitPoint == NULL) {
            return 0;
        }

    }

    return 1;
}

int hasBeenDestroyed(GameStorage *storage, PieceInBoard *board) {

    if (isDestroyed(board)) {
        return 1;
    }

    int destroyed = 0;

    switch (storage->type) {

        case GS_MATRIX:
            destroyed = hasBeenDestroyedMatrix(storage->data.matrix, board);
            break;
        case GS_QUAD:
            destroyed = hasBeenDestroyedQuad(storage->data.quadTree, board);
            break;
    }

    if (destroyed) {
        setDestroyed(board);
    }

    return destroyed;
}

void iterateAllStoredPoints(GameStorage *storage, void (*toCall)(PointStorage *)) {

    void (*toCallCasted)(void *) = (void (*)(void *)) toCall;

    switch (storage->type) {

        case GS_QUAD:
            q_iterateAllPoints(storage->data.quadTree, toCallCasted);
            break;
        case GS_MATRIX:
            m_iterateAllPoints(storage->data.matrix, toCallCasted);
            break;
    }

}