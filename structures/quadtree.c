#include "quadtree.h"
#include <stdlib.h>
#include <stdio.h>

#define NODE_MAX_CHILDREN 4

typedef enum {
    I_SUCCESS,
    I_FAILED_NOT_INSIDE,
    I_FAILED_FULL
} INSERT_RESULT;

struct QuadNode_ *initLeaf(Position *, Position *);

int isContainedInside(Position *, Position *topLeft, Position *bottomRight);

struct QuadNode_ *divideLeafNode(struct QuadNode_ *);

INSERT_RESULT insertIntoNode(struct Node_ *node, QuadPoint *pos);

INSERT_RESULT insertIntoLeaf(struct LeafNode_ *node, QuadPoint *pos);

QuadPoint *lookupQuad(struct QuadNode_ *node, Position *pos);

QuadPoint *lookupNode(struct Node_ *node, Position *pos);

QuadPoint *lookupLeaf(struct LeafNode_ *node, Position *pos);

void freeQuadNode(struct QuadNode_ *node);


QuadTree *initQuadTree(Position *topLeft, Position *bottomRight) {

    QuadTree *quadTree = malloc(sizeof(QuadTree));

    quadTree->rootNode = initLeaf(topLeft, bottomRight);

    return quadTree;
}

QuadPoint *initQuadPoint(Position *pos, void *value) {

    QuadPoint *quadPoint = malloc(sizeof(QuadPoint));

    quadPoint->pos = clonePos(pos);

    quadPoint->value = value;

    return quadPoint;
}

struct QuadNode_ *initQuadNode(Position *topLeft, Position *bottomRight, NodeKind kind, void *nodeValue) {

    struct QuadNode_ *node = malloc(sizeof(struct QuadNode_));

    node->topLeft = clonePos(topLeft);
    node->bottomRight = clonePos(bottomRight);

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

struct QuadNode_ *initNode(Position *topLeft, Position *bottomRight) {

    struct Node_ *node = malloc(sizeof(struct Node_));

    node->children = malloc(NODE_MAX_CHILDREN * sizeof(struct QuadTree_ *));

    int midX = (p_getBaseX(topLeft) + p_getBaseX(bottomRight)) / 2,
            midY = (p_getBaseY(topLeft) + p_getBaseY(bottomRight)) / 2;

    Position *mid = initPos(midX, midY),
            *topMid = initPos(midX, p_getBaseY(topLeft)),
            *midRight = initPos(p_getBaseX(bottomRight), midY),
            *midLeft = initPos(p_getBaseX(topLeft), midY),
            *bottomMid = initPos(midX, p_getBaseY(bottomRight));

    struct QuadNode_ *topLeftChild = initLeaf(topLeft, mid),
            *topRightChild = initLeaf(topMid, midRight),
            *bottomLeftChild = initLeaf(midLeft, bottomMid),
            *bottomRightChild = initLeaf(mid, bottomRight);

    node->children[0] = topLeftChild;
    node->children[1] = topRightChild;
    node->children[2] = bottomLeftChild;
    node->children[3] = bottomRightChild;

    p_free(mid);
    p_free(topMid);
    p_free(midRight);
    p_free(midLeft);
    p_free(bottomMid);

    node->stored = 4;

    return initQuadNode(topLeft, bottomRight, QT_NODE, node);
}

struct QuadNode_ *initLeaf(Position *topLeft, Position *bottomRight) {

    struct LeafNode_ *leaf = malloc(sizeof(struct LeafNode_));

    leaf->positions = malloc(MAX_NODES_PER_LEAF * sizeof(QuadPoint *));

    leaf->stored = 0;

    return initQuadNode(topLeft, bottomRight, QT_LEAF, leaf);
}

INSERT_RESULT insertNode(struct QuadNode_ *node, QuadPoint *pos) {

    if (!isContainedInside(pos->pos, node->topLeft, node->bottomRight)) {
        return I_FAILED_NOT_INSIDE;
    }

    switch (node->nodeKind) {

        case QT_LEAF: {
            return insertIntoLeaf(node->leaf, pos);
        }

        case QT_NODE: {
            return insertIntoNode(node->node, pos);
        }
    }

}

INSERT_RESULT insertIntoNode(struct Node_ *node, QuadPoint *pos) {

    INSERT_RESULT result = I_FAILED_NOT_INSIDE;

    for (int i = 0; i <= node->stored; i++) {

        result = insertNode(node->children[i], pos);

        if (result == I_SUCCESS) {
            break;
        } else if (result == I_FAILED_FULL) {

            node->children[i] = divideLeafNode(node->children[i]);

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

void insert(QuadTree *quad, QuadPoint *pos) {

    struct QuadNode_ *node = quad->rootNode;

    INSERT_RESULT result = insertNode(node, pos);

    if (result == I_FAILED_FULL) {

        //Divide the node
        quad->rootNode = divideLeafNode(node);

        insert(quad, pos);
    }

}

QuadPoint *lookup(QuadTree* quad, Position *pos) {

    struct QuadNode_ *node = quad->rootNode;

    return lookupQuad(node, pos);
}

QuadPoint *lookupQuad(struct QuadNode_ *node, Position *pos) {

    if (!isContainedInside(pos, node->topLeft, node->bottomRight)) return NULL;

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

    for (int i = 0; i < node->stored; i ++) {

        QuadPoint *result = lookupQuad(node->children[i], pos);

        if (result != NULL) {
            return result;
        }

    }

    return NULL;
}

QuadPoint *lookupLeaf(struct LeafNode_ *node, Position *pos) {

    for (int i = 0; i < node->stored; i++) {

        QuadPoint * point = node->positions[i];

        if (p_comparePositions(point->pos, pos)) {
            return point;
        }

    }

    return NULL;
}

struct QuadNode_ *divideLeafNode(struct QuadNode_ *node) {

    if (node->nodeKind != QT_LEAF) {
        fprintf(stderr, "Attempting to divide a node that isn't a leaf.");

        exit(1);
    }

    struct QuadNode_ *newNode = initNode(node->topLeft, node->bottomRight);

    for (int i = 0; i < node->leaf->stored; i++) {

        insertNode(newNode, node->leaf->positions[i]);

    }

    freeQuadNode(node);

    return newNode;
}

int isContainedInside(Position *point, Position *topLeft, Position *bottomRight) {

    int x = p_getBaseX(point), y = p_getBaseY(point);

    int tX = p_getBaseX(topLeft), tY = p_getBaseY(topLeft),
            bX = p_getBaseX(bottomRight), bY = p_getBaseY(bottomRight);

    return tX <= x && x <= bX && bY <= y && y <= tY;

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

    p_free(node->topLeft);
    p_free(node->bottomRight);

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

    free(leaf->positions);

    free(leaf);

}

void freeQuad(QuadTree *qt) {

    freeQuadNode(qt->rootNode);

    free(qt);

}

void freeQuadPoint(QuadPoint *qp) {

    p_free(qp->pos);

    free(qp);

}