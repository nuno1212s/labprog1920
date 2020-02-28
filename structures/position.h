#ifndef LABPROG_POSITION_H
#define LABPROG_POSITION_H

typedef enum {
    P_UP,
    P_DOWN,
    P_LEFT,
    P_RIGHT
} PlacedDirection;

struct Position_ {

    int x, y;

};

typedef struct Position_ Position;

Position *initPos(int, int);

Position *clonePos(Position *);

int p_getBaseX(Position *);

int p_getBaseY(Position *);

int p_comparePositions(Position *, Position *);

Position *addTo(Position *, Position *);

Position *addToCoords(Position *, int x, int y);

Position *addToWithDirection(Position *, Position *, PlacedDirection);

void p_free(Position *);

#endif //LABPROG_POSITION_H
