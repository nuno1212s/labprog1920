#include "position.h"
#include <stdlib.h>
#include <stdio.h>

Position *initPos(int baseX, int baseY) {

    Position *pos = malloc(sizeof(Position));

    pos->x = baseX;

    pos->y = baseY;

    return pos;
}

Position *clonePos(Position *p) {
    return initPos(p_getBaseX(p), p_getBaseY(p));
}

int p_comparePositions(Position *p1, Position *p2) {

    return p_getBaseX(p1) == p_getBaseX(p2) && p_getBaseY(p1) == p_getBaseY(p2);

}

Position *addTo(Position *pos, Position *pos2) {

    pos->x += p_getBaseX(pos2);

    pos->y += p_getBaseY(pos2);

    return pos;
}

Position *addToCoords(Position *pos, int x, int y) {

    pos->x += x;
    pos->y += y;

    return pos;
}

int p_sin(PlacedDirection direction) {

    switch (direction) {

        case P_UP:
            return 1;
        case P_DOWN:
            return -1;
        default:
            return 0;

    }

}

int p_cos(PlacedDirection direction) {

    switch (direction) {

        case P_RIGHT:
            return 1;
        case P_LEFT:
            return -1;

        default:
            return 0;

    }

}

Position *addToWithDirection(Position *pos, Position *pos2, PlacedDirection placedDirection) {

    printf("Before rotation: %d, %d\n", p_getBaseX(pos), p_getBaseY(pos));

    pos->x = pos->x * p_cos(placedDirection) + pos->y * p_sin(placedDirection);
    pos->y = pos->x * p_sin(placedDirection) + pos->y * p_cos(placedDirection);

    printf("After rotation: %d %d %d\n", p_getBaseX(pos), p_getBaseY(pos), placedDirection);

    pos->x += p_getBaseX(pos2);
    pos->y += p_getBaseY(pos2);

    printf("After all: %d %d\n", p_getBaseX(pos), p_getBaseY(pos));

    return pos;
}

void p_free(Position *p) {
    free(p);
}