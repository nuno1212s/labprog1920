#ifndef LABPROG_POSITION_H
#define LABPROG_POSITION_H

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} PLACED_DIRECTION;

struct Position_ {

    int baseX, baseY;

    PLACED_DIRECTION direction;
};

typedef struct Position_ Position;

Position *initPos(int baseX, int baseY, PLACED_DIRECTION dir);

int p_getBaseX(Position *);

int p_getBaseY(Position *);

PLACED_DIRECTION p_getDirection(Position *);

void p_free(Position *);

#endif //LABPROG_POSITION_H
