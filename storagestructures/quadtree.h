#ifndef LABPROG_QUADTREE_H
#define LABPROG_QUADTREE_H

#include "../structures/position.h"

#define MAX_NODES_PER_LEAF 1

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

/**
 * Creates a quadTree with the given topLeft and bottomRight positions
 *
 * These positions are cloned so they can be safely deleted afterwards
 * @return
 */
QuadTree *initQuadTree(Position *topLeft, Position *bottomRight);

void *getValue(QuadPoint *);

void qt_insert(QuadTree *, Position *, void *);

void* qt_lookup(QuadTree *, Position *);

void* qt_delete(QuadTree *, Position *);

void q_iterateAllPoints(QuadTree *, void (*)(void *));

void freeQuad(QuadTree *);

void printQuad(QuadTree *);

#endif