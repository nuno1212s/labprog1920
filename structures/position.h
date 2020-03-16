#ifndef LABPROG_POSITION_H
#define LABPROG_POSITION_H

#define p_getBaseX(p) (p->x)
#define p_getBaseY(p) (p->y)

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

/**
 * Compares two positions
 * @return != 0 If the positions match, 0 if they don't
 */
int p_comparePositions(Position *, Position *);

Position *addTo(Position *, Position *);

Position *addToCoords(Position *, int x, int y);

Position *addToWithDirection(Position *, Position *, PlacedDirection);

void p_free(Position *);

#endif //LABPROG_POSITION_H
