#include "quadtree.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define NODE_MAX_CHILDREN 4

typedef enum {
    I_SUCCESS,
    I_FAILED_NOT_INSIDE,
    I_FAILED_FULL
} INSERT_RESULT;

static struct QuadNode_ *initLeaf(Position *, int);

static int isContainedInside(Position *, Position *bottomLeft, int dimension);

static struct QuadNode_ *divideLeafNode(struct QuadNode_ *);

static INSERT_RESULT insertIntoNode(struct Node_ *node, QuadPoint *pos);

static INSERT_RESULT insertIntoLeaf(struct LeafNode_ *node, QuadPoint *pos);

static QuadPoint *initQuadPoint(Position *, void *);

static void freeQuadPoint(QuadPoint *);

static QuadPoint *lookupQuad(struct QuadNode_ *node, Position *pos);

static QuadPoint *lookupNode(struct Node_ *node, Position *pos);

static QuadPoint *lookupLeaf(struct LeafNode_ *node, Position *pos);

static QuadPoint *deleteQuad(struct QuadNode_ *, Position *);

void freeQuadNode(struct QuadNode_ *node);

QuadTree *initQuadTree(int dimensions) {

    QuadTree *quadTree = malloc(sizeof(QuadTree));

    //Use powers of 2 to make it divisible all the way to 0
    int closestNextPowerOf2 = (int) pow(2, (int) sqrt(dimensions) + 1);

    Position bottomLeft = {0, 0};

    quadTree->rootNode = initLeaf(&bottomLeft, closestNextPowerOf2);

    return quadTree;
}

QuadPoint *initQuadPoint(Position *pos, void *value) {

    QuadPoint *quadPoint = malloc(sizeof(QuadPoint));

    quadPoint->pos = clonePos(pos);

    quadPoint->value = value;

    return quadPoint;
}

void *getValue(QuadPoint *point) {
    return point->value;
}

struct QuadNode_ *initQuadNode(Position *bottomLeft, int dimension, NodeKind kind, void *nodeValue) {

    struct QuadNode_ *node = malloc(sizeof(struct QuadNode_));

    node->bottomLeft = clonePos(bottomLeft);

    node->dimension = dimension;

    node->nodeKind = kind;

    switch (kind) {
        case QT_LEAF:
            node->leaf = nodeValue;
            break;
        case QT_NODE:
            node->node = nodeValue;
            break;

        default:
            break;
    }

    return node;
}

static int directions[NODE_MAX_CHILDREN][2] = {{1, 1},
                                               {0, 1},
                                               {0, 0},
                                               {1, 0}};

/**
 * Creates a middle node that contains 4 children.
 *
 * @param bottomLeft
 * @param dimension
 * @return
 */
struct QuadNode_ *initNode(Position *bottomLeft, int dimension) {

    struct Node_ *node = malloc(sizeof(struct Node_));

    node->children = malloc(NODE_MAX_CHILDREN * sizeof(struct QuadTree_ *));

    //Allocate positions in the stack as it's much faster
    //And these positions are disposable anyway
    Position currentBottomLeft = {p_getBaseX(bottomLeft), p_getBaseY(bottomLeft)};

    int childDimension = dimension / 2;

    for (int i = 0; i < NODE_MAX_CHILDREN; i++) {

        Position newBottomLeft = {p_getBaseX((&currentBottomLeft)) + directions[i][0] * childDimension,
                                  p_getBaseY((&currentBottomLeft)) + directions[i][1] * childDimension};

        node->children[i] = initLeaf(&newBottomLeft, childDimension);
    }

    node->stored = NODE_MAX_CHILDREN;

    return initQuadNode(bottomLeft, dimension, QT_NODE, node);
}

struct QuadNode_ *initLeaf(Position *bottomLeft, int dimension) {

    struct LeafNode_ *leaf = malloc(sizeof(struct LeafNode_));

    leaf->positions = malloc(MAX_NODES_PER_LEAF * sizeof(QuadPoint *));

    leaf->stored = 0;

    return initQuadNode(bottomLeft, dimension, QT_LEAF, leaf);
}

INSERT_RESULT insertNode(struct QuadNode_ *node, QuadPoint *pos) {

    if (!isContainedInside(pos->pos, node->bottomLeft, node->dimension)) {
        return I_FAILED_NOT_INSIDE;
    }

    switch (node->nodeKind) {

        case QT_LEAF: {

//            printf("INSERTED INTO %d %d %d %d\n", p_getBaseX(node->bottomLeft),
//                    p_getBaseY(node->bottomLeft),
//                    p_getBaseX(node->bottomLeft) + node->dimension,
//                    p_getBaseY(node->bottomLeft) + node->dimension);

            return insertIntoLeaf(node->leaf, pos);
        }

        case QT_NODE: {
            return insertIntoNode(node->node, pos);
        }
    }

}

INSERT_RESULT insertIntoNode(struct Node_ *node, QuadPoint *pos) {

    INSERT_RESULT result = I_FAILED_NOT_INSIDE;

    for (int i = 0; i < node->stored; i++) {

        result = insertNode(node->children[i], pos);

        if (result == I_SUCCESS) {
            break;
        } else if (result == I_FAILED_FULL) {

            node->children[i] = divideLeafNode(node->children[i]);

            //After dividing try again
            i--;
            continue;
        }

    }

    return result;
}

INSERT_RESULT insertIntoLeaf(struct LeafNode_ *node, QuadPoint *pos) {

    if (node->stored < MAX_NODES_PER_LEAF) {

        node->positions[node->stored++] = pos;

        return I_SUCCESS;
    }

    return I_FAILED_FULL;
}

void qt_insert(QuadTree *quad, Position *pos, void *value) {

//    printf("INSERTING POINT %d %d\n", p_getBaseX(pos), p_getBaseY(pos));

    struct QuadNode_ *node = quad->rootNode;

    QuadPoint *point = qt_lookup(quad, pos);

    if (point == NULL) {
        point = initQuadPoint(pos, value);
    } else {
        point->value = value;
        //Point was already there, just had the refresh the value of it
        return;
    }

    INSERT_RESULT result = insertNode(node, point);

    if (result == I_FAILED_FULL) {

        //Divide the node
        quad->rootNode = divideLeafNode(node);

        insertNode(quad->rootNode, point);
    }

}

void *qt_lookup(QuadTree *quad, Position *pos) {

    struct QuadNode_ *node = quad->rootNode;

    QuadPoint *point = lookupQuad(node, pos);

    if (point != NULL) {
        return point->value;
    }
    return NULL;
}

QuadPoint *lookupQuad(struct QuadNode_ *node, Position *pos) {

    if (!isContainedInside(pos, node->bottomLeft, node->dimension)) return NULL;

    switch (node->nodeKind) {

        case QT_NODE: {
            return lookupNode(node->node, pos);
        }

        case QT_LEAF: {
            return lookupLeaf(node->leaf, pos);
        }

    }

}

QuadPoint *lookupNode(struct Node_ *node, Position *pos) {

    for (int i = 0; i < node->stored; i++) {

        QuadPoint *result = lookupQuad(node->children[i], pos);

        if (result != NULL) {
            return result;
        }

    }

    return NULL;
}

QuadPoint *lookupLeaf(struct LeafNode_ *node, Position *pos) {

    for (int i = 0; i < node->stored; i++) {

        QuadPoint *point = node->positions[i];

        if (p_comparePositions(point->pos, pos)) {
            return point;
        }

    }

    return NULL;
}

void *qt_delete(QuadTree *quad, Position *pos) {

    QuadPoint *point = deleteQuad(quad->rootNode, pos);

    if (point != NULL) {

        void *value = point->value;

        freeQuadPoint(point);

        return value;
    }

    return NULL;
}

QuadPoint *deleteLeaf(struct LeafNode_ *node, Position *pos) {

    int found = 0;

    QuadPoint *point = NULL;

    for (int i = 0; i < node->stored; i++) {
        if (found) {
            node->positions[i - 1] = node->positions[i];
            continue;
        }

        if (p_comparePositions(pos, node->positions[i]->pos)) {

            found = 1;

            point = node->positions[i];

            node->positions[i] = NULL;
        }
    }

    if (found) {
        node->stored--;
    }

    return point;
}

QuadPoint *deleteNode(struct Node_ *node, Position *pos) {

    for (int i = 0; i < node->stored; i++) {

        QuadPoint *point = NULL;

        if ((point = deleteQuad(node->children[i], pos)) != NULL) {
            return point;
        }

    }

    return NULL;
}

QuadPoint *deleteQuad(struct QuadNode_ *node, Position *pos) {

    if (!isContainedInside(pos, node->bottomLeft, node->dimension)) return NULL;

    switch (node->nodeKind) {

        case QT_NODE: {
            return deleteNode(node->node, pos);
        }

        case QT_LEAF: {
            return deleteLeaf(node->leaf, pos);
        }
    }

    return NULL;
}


struct QuadNode_ *divideLeafNode(struct QuadNode_ *node) {

    if (node->nodeKind != QT_LEAF) {
        fprintf(stderr, "Attempting to divide a node that isn't a leaf.");

        exit(1);
    }

    struct QuadNode_ *newNode = initNode(node->bottomLeft, node->dimension);

    //Move the stored points into the new nodes
    for (int i = 0; i < node->leaf->stored; i++) {

        insertNode(newNode, node->leaf->positions[i]);

        node->leaf->positions[i] = NULL;
        node->leaf->stored--;
        //We don't want the nodes getting freed afterwards
    }

    freeQuadNode(node);

    return newNode;
}

int isContainedInside(Position *point, Position *bottomLeft, int dimension) {

    int x = p_getBaseX(point), y = p_getBaseY(point);

    int base_X = p_getBaseX(bottomLeft), base_Y = p_getBaseY(bottomLeft),
            large_X = base_X + dimension, large_Y = base_Y + dimension;

//    printf("Checking if %d, %d is in %d, %d -> %d %d\n", x, y, base_X, base_Y, large_X, large_Y);

    return base_X <= x && x < large_X && base_Y <= y && y < large_Y;
}

void disposeOfNode(struct Node_ *);

void disposeOfLeafNode(struct LeafNode_ *);

void freeQuadNode(struct QuadNode_ *node) {

    switch (node->nodeKind) {

        case QT_LEAF: {
            disposeOfLeafNode(node->leaf);

            break;
        }

        case QT_NODE: {
            disposeOfNode(node->node);

            break;
        }
    }

    p_free(node->bottomLeft);

    free(node);
}

void disposeOfNode(struct Node_ *node) {

    for (int i = 0; i < node->stored; i++) {

        freeQuadNode(node->children[i]);

    }

    free(node->children);

    free(node);

}

void disposeOfLeafNode(struct LeafNode_ *leaf) {

    for (int i = 0; i < leaf->stored; i++) {
        freeQuadPoint(leaf->positions[i]);
    }

    free(leaf->positions);

    free(leaf);

}

void iterateAllPointsNode(struct QuadNode_ *node, void (*toCall)(void *));

void iterateAllPointsQtNode(struct Node_ *node, void (*toCall)(void *)) {

    for (int i = 0; i < node->stored; i++) {

        iterateAllPointsNode(node->children[i], toCall);

    }

}

void iterateAllPointQtLeaf(struct LeafNode_ *leaf, void (*toCall)(void *)) {

//    printf("Stored: %d\n", leaf->stored);

    for (int i = 0; i < leaf->stored; i++) {
        toCall(leaf->positions[i]->value);
    }
}

void iterateAllPointsNode(struct QuadNode_ *node, void (*toCall)(void *)) {

    switch (node->nodeKind) {
        case QT_LEAF:
            iterateAllPointQtLeaf(node->leaf, toCall);
            break;

        case QT_NODE:
            iterateAllPointsQtNode(node->node, toCall);
            break;
    }

}

void q_iterateAllPoints(QuadTree *tree, void (*toCall)(void *)) {

    iterateAllPointsNode(tree->rootNode, toCall);

}

void freeQuad(QuadTree *qt) {

    freeQuadNode(qt->rootNode);

    free(qt);

}

void freeQuadPoint(QuadPoint *qp) {

//    printf("Freeing point %p\n", qp);

    p_free(qp->pos);

    free(qp);

}

void printFullNode(struct QuadNode_ *);

void printQuadNode(struct Node_ *quadNode) {

    for (int i = 0; i < quadNode->stored; i++) {
        printf("NODE-");
        printFullNode(quadNode->children[i]);
    }

    printf("\n");

}

void printQuadLeaf(struct LeafNode_ *quadLeaf) {

    for (int i = 0; i < quadLeaf->stored; i++) {

        QuadPoint *qp = quadLeaf->positions[i];

        printf("(%d, %d, %p) \n", p_getBaseX(qp->pos), p_getBaseY(qp->pos), qp->value);
    }

}

void printFullNode(struct QuadNode_ *qn) {

    switch (qn->nodeKind) {

        case QT_LEAF:
            printQuadLeaf(qn->leaf);
            break;
        case QT_NODE:
            printQuadNode(qn->node);
            break;

    }
}

void printQuad(QuadTree *qt) {
    printFullNode(qt->rootNode);
}