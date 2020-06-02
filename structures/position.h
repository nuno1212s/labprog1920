#ifndef LABPROG_POSITION_H
#define LABPROG_POSITION_H

#define p_getBaseX(p) (p->x)
#define p_getBaseY(p) (p->y)
#define p_setX(p, x) (p->x = x)
#define p_setY(p, y) (p->y = y)

typedef enum {
    P_UP,//UP is 0º
    P_DOWN,//DOWN is 180º
    P_LEFT,//LEFT is 90º
    P_RIGHT//RIGHT is 270º
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

/**
 * Add the given coordinates to the position
 *
 * @param x
 * @param y
 * @return
 */
Position *addToCoords(Position *, int x, int y);

/**
 * Add the second position to the first position, with the given direction.
 *
 * The first position is modified and returned
 *
 * @return
 */
Position *addToWithDirection(Position *, Position *, PlacedDirection);

void printPos(Position *);

void p_free(Position *);

#endif //LABPROG_POSITION_H
