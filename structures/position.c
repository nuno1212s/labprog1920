#include "position.h"
#include <stdlib.h>

Position* initPos(int baseX, int baseY) {

    Position *pos = malloc(sizeof(Position));

    pos->x = baseX;

    pos->y = baseY;

    return pos;
}

int p_getBaseX(Position *p) {
    return p->x;
}

int p_getBaseY(Position *p) {
    return p->y;
}


void p_free(Position *p) {
    free(p);
}