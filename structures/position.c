#include "position.h"
#include <stdlib.h>

Position* initPos(int baseX, int baseY) {

    Position *pos = malloc(sizeof(Position));

    pos->x = baseX;

    pos->y = baseY;

    return pos;
}

Position *clonePos(Position *p) {
    return initPos(p_getBaseX(p), p_getBaseY(p));
}

int p_getBaseX(Position *p) {
    return p->x;
}

int p_getBaseY(Position *p) {
    return p->y;
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

Position *addToWithDirection(Position *pos, Position *pos2, PlacedDirection placedDirection) {

    switch (placedDirection) {

        case P_UP: {

            pos->x += p_getBaseX(pos2);
            pos->y += p_getBaseY(pos2);

            break;
        }

        case P_DOWN: {

            pos->x += p_getBaseX(pos2) * -1;
            pos->y += p_getBaseY(pos2);

            break;
        }

        case P_LEFT: {

            //90º counter clock wise rotation
            pos->x += p_getBaseY(pos2) * -1;
            pos->y += p_getBaseX(pos2);

            break;
        }

        case P_RIGHT: {

            pos->x += p_getBaseY(pos2);
            pos->y += p_getBaseX(pos2);

            break;
        }

    }

    return pos;
}

void p_free(Position *p) {
    free(p);
}