#ifndef LABPROG_GAME_H
#define LABPROG_GAME_H

#include "position.h"
#include "gamestructures.h"

struct PossiblePieces_ {

    int count;

    Piece **pieces;

};

typedef struct PossiblePieces_ PossiblePieces;


#endif //LABPROG_GAME_H
