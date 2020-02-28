#ifndef LABPROG_QUADTREE_H
#define LABPROG_QUADTREE_H

#include "position.h"

#define MAX_NODES_PER_LEAF 5

typedef enum {
    QT_NODE, QT_LEAF
} NodeKind;

struct QuadTree_ {

    struct QuadNode_ *rootNode;

};

struct QuadNode_ {

    Position *topLeft, *bottomRight;

    NodeKind nodeKind;

    union {

        struct Node_ *node;

        struct LeafNode_ *leaf;

    };

};

struct Node_ {

    int stored;

    struct QuadNode_ **children;

};

struct LeafNode_ {

    int stored;

    struct QuadPoint_ **positions;

};

struct QuadPoint_ {

    Position *pos;

    void *value;

};

typedef struct QuadPoint_ QuadPoint;

typedef struct QuadTree_ QuadTree;

QuadTree *initQuadTree(Position *, Position *);

QuadPoint* initQuadPoint(Position *, void *);

void insert(QuadTree *, QuadPoint *);

QuadPoint* lookup(QuadTree *, Position *);

QuadPoint* delete(QuadTree *, Position *);

void freeQuadPoint(QuadPoint *);

void freeQuad(QuadTree *);

#endif