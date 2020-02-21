#include "position.h"
#include <stdlib.h>

Position* initPos(int baseX, int baseY, PLACED_DIRECTION dir) {

    Position *pos = malloc(sizeof(Position));

    pos->baseX = baseX;

    pos->baseY = baseY;

    pos->direction = dir;

    return pos;
}

int p_getBaseX(Position *p) {
    return p->baseX;
}

int p_getBaseY(Position *p) {
    return p->baseY;
}

PLACED_DIRECTION p_getDirection(Position *p) {
    return p->direction;
}

void p_free(Position *p) {
    free(p);
}