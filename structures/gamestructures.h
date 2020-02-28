#ifndef LABPROG_GAMESTRUCTURES_H
#define LABPROG_GAMESTRUCTURES_H

#include "position.h"
#include "quadtree.h"

struct Matrix_ {

};

struct PointStorage_ {

    Position *pointPos;

    struct PieceInBoard_ *piece;

    struct HitPoint_ *hitPoint;

};

struct Piece_ {

    int size;

    char *name;

};

struct PieceInBoard_ {

    struct Piece_ *piece;

    Position *basePos;

    PlacedDirection direction;

};

struct HitPoint_ {

    Position *pos;

    int hit;

};

struct GameStorage_ {

    enum {
        GS_MATRIX,
        GS_QUAD
    } type;

    union {
        struct Matrix_ matrix;

        struct QuadTree_ quadTree;
    } data;
};

typedef struct Piece_ Piece;

typedef struct PieceInBoard_ PieceInBoard;

typedef struct HitPoint_ HitPoint;

typedef struct GameStorage_ GameStorage;

HitPoint *initHitPoint(Position *, int);

int gs_hasHit(HitPoint *);

Position *gs_getPos(HitPoint *);

void gs_freeHP(HitPoint *);

PieceInBoard *initPieceInBoard(Piece *, Position *, PlacedDirection dir);

Piece *gs_getPieceType(PieceInBoard *);

Position *gs_getBasePosition(PieceInBoard *);

PlacedDirection gs_getPlacedDir(PieceInBoard *);

void gs_freePIB(PieceInBoard *);

#endif //LABPROG_GAMESTRUCTURES_H
