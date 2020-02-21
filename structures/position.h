#ifndef LABPROG_POSITION_H
#define LABPROG_POSITION_H

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} PLACED_DIRECTION;

struct Position_ {

    int x, y;
};

typedef struct Position_ Position;

Position *initPos(int, int);

int p_getBaseX(Position *);

int p_getBaseY(Position *);

void p_free(Position *);

#endif //LABPROG_POSITION_H
