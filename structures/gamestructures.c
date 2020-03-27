#include "gamestructures.h"
#include <stdlib.h>
#include <string.h>

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

    int pos = 0;

    for (int x = 0; x < matrix_rows(piece->matrix); x++) {

        for (int y = 0; y < matrix_cols(piece->matrix); y++) {

            if (m_getBit(piece->matrix, x, y)) {

                newPositions[pos++] = addToWithDirection(initPos(x, y), base, dir);

            }
        }
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
            toLookFunc = (void *(*)(void *, Position *)) qt_lookup;

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
            PointStorage *ps = initPS(NULL, board, NULL);

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

            PointStorage *ps = initPS(NULL, board, NULL);

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

void removePlayedPieceMatrix(Matrix *matrix, PieceInBoard *board) {

    Position **pos = calculateNewPositions(board->piece, board->basePos, board->direction);

    for (int i = 0; i < board->piece->size;i ++) {

        void *result = m_lookup(matrix, pos[i]);

        if (result == NULL) {
            continue;
        }

        PointStorage *p = (PointStorage *) result;

        freePS(p);

        m_delete(matrix, pos[i]);
    }

}

void removePlayedPieceQuad(QuadTree* qt, PieceInBoard *piece) {

    Position **pos = calculateNewPositions(piece->piece, piece->basePos, piece->direction);

    for (int i = 0; i < piece->piece->size; i++) {

        void *result = qt_lookup(qt, pos[i]);

        if (result == NULL) continue;

        freePS(result);
        //We can just delete the point because to remove played pieces, we know that the game has not started yet and no player has played yet
        qt_delete(qt, pos[i]);
    }

}

void removePlayedPiece(GameStorage *gs, PieceInBoard *board) {

    switch (gs->type) {
        case GS_MATRIX:
            removePlayedPieceMatrix(gs->data.matrix, board);
            return;
        case GS_QUAD:
            removePlayedPieceQuad(gs->data.quadTree, board);
            return;
    }

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

        PointStorage *storage = initPS(initHitPoint(toHit, 0), NULL, NULL);

        qt_insert(tree, toHit, storage);

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

HitResponse attemptHitMatrix(Matrix *matrix, Position *toHit) {

    PointStorage *point = m_lookup(matrix, toHit);

    if (point == NULL) {

        m_insert(matrix, toHit, initPS(initHitPoint(toHit, 0), NULL, NULL));

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
        incrementDestroyed(s.hit);
    }

    return s;
}

void registerHitMatrix(Matrix *matrix, Position *pos, int result) {

    void *position = m_lookup(matrix, pos);

    HitPoint *ourHitPoint = initHitPoint(pos, result);

    if (position == NULL) {
        position = initPS(NULL, NULL, ourHitPoint);

        m_insert(matrix, pos, position);
    } else {

        PointStorage *ps = (PointStorage *) position;

        ps->ownHitPoint = ourHitPoint;
    }

}

void registerHitQuad(QuadTree *tree, Position *pos, int result) {

    void *position = qt_lookup(tree, pos);

    HitPoint *ourHitPoint = initHitPoint(pos, result);

    if (position == NULL) {
        position = initPS(NULL, NULL, ourHitPoint);

        qt_insert(tree, pos, position);
    } else {

        PointStorage *ps = (PointStorage *) position;

        ps->ownHitPoint = ourHitPoint;
    }
}

void registerHit(GameStorage *storage, Position *pos, int result) {

    switch (storage->type) {

        case GS_MATRIX:
            registerHitMatrix(storage->data.matrix, pos, result);
            break;

        case GS_QUAD:
            registerHitQuad(storage->data.quadTree, pos, result);
            break;
    }

}

int hasPlayedGeneric(void *storage, Position *pos, void *(*toLook)(void *, Position *)) {

    void *result = toLook(storage, pos);

    if (result == NULL) {
        return 0;
    } else if (((PointStorage *) result)->ownHitPoint == NULL) {
        return 0;
    }

    return 1;
}

int hasPlayed(GameStorage *storage, Position *pos) {

    void * (*toLook)(void*, Position *) = NULL;

    void *actualStorage = NULL;

    switch (storage->type) {
        case GS_QUAD:
            toLook = (void*(*)(void*, Position *)) qt_lookup;

            actualStorage = storage->data.quadTree;
            break;
        case GS_MATRIX:
            toLook = (void*(*)(void *, Position *)) m_lookup;

            actualStorage = storage->data.matrix;
            break;
    }

    return hasPlayedGeneric(actualStorage, pos, toLook);

}

int hasBeenDestroyed(GameStorage *storage, PieceInBoard *board) {
    return isDestroyed(board);
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