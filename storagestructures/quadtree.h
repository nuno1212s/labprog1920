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

    Position *bottomLeft;

    int dimension;

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
 * Creates a quadTree with the given dimensions
 * @return
 */
QuadTree *initQuadTree(int dimensions);

/**
 * Gets the value stored in a Quad Point
 * @return
 */
void *getValue(QuadPoint *);

/**
 * Insert a value into the given quad tree and position
 */
void qt_insert(QuadTree *, Position *, void *);

/**
 * Lookup the value that is stored in a quadtree position
 * @return
 */
void* qt_lookup(QuadTree *, Position *);

/**
 * Delete the value that is stored in the quad tree in the given position
 * @return
 */
void* qt_delete(QuadTree *, Position *);

/**
 * Iterate all points of the quad tree and apply a function to them
 */
void q_iterateAllPoints(QuadTree *, void (*)(void *));

/**
 * Free the memory taken by the quad tree.
 *
 * This does not free all the data that is stored in the quadtree, only the tree structure itself.
 */
void freeQuad(QuadTree *);

void printQuad(QuadTree *);

#endif