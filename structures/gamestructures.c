#include "gamestructures.h"
#include <stdlib.h>
#include <string.h>

PointStorage* initPS(HitPoint *point, PieceInBoard* board) {

    PointStorage *ps = malloc(sizeof(PointStorage));

    ps->piece = board;
    ps->hitPoint = point;

    return ps;
}

void freePS(PointStorage *ps) {

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

int insertPieceQuad(QuadTree *qt, Piece *piece, Position *basePos, PlacedDirection dir) {

    Position **pos = calculateNewPositions(piece, basePos, dir);

    for (int i = 0; i < piece->size; i++) {

        void *point = qt_lookup(qt, pos[i]);

        if (point == NULL) continue;

        PointStorage *storage = (PointStorage *) point;

        /*
         * There is already a piece stored in the positions that are occupied by this new piece
         */
        if (storage->piece == NULL) continue;

        freePositions(pos, piece->size);

        return 0;
    }

    PieceInBoard *board = initPieceInBoard(piece, basePos, dir);

    for (int i = 0; i < piece->size; i++) {
        PointStorage *ps = initPS(NULL, board);

        qt_insert(qt, pos[i], ps);
    }

    return 1;
}

int insertPieceMatrix(Matrix *matrix, Piece *piece, Position *basePos, PlacedDirection dir) {
    Position **pos = calculateNewPositions(piece, basePos, dir);

    for (int i = 0; i < piece->size; i++) {
        void *point = m_lookup(matrix, pos[i]);

        if (point == NULL) continue;

        PointStorage *storage = (PointStorage *) point;

        if (storage->piece == NULL) continue;

        freePositions(pos, piece->size);

        return 0;
    }

    PieceInBoard *board = initPieceInBoard(piece, basePos, dir);

    for (int i = 0; i < piece->size; i++) {
        PointStorage *ps = initPS(NULL, board);

        m_insert(matrix, pos[i], ps);
    }

    return 1;
}

int insertPiece(GameStorage *gs, Piece *piece, Position *basePos, PlacedDirection dir) {

    switch (gs->type) {

        case GS_MATRIX: {
            return insertPieceMatrix(gs->data.matrix, piece, basePos, dir);
        }

        case GS_QUAD: {
            return insertPieceQuad(gs->data.quadTree, piece, basePos, dir);
        }

    }

    return 0;
}