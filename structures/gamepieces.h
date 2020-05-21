//
// Created by nuno on 21/05/20.
//

#ifndef LABPROG_GAMEPIECES_H
#define LABPROG_GAMEPIECES_H

#include "../storagestructures/linkedlist.h"

#include "pieces.h"

struct Game_;

struct PossiblePieces_ {

    LinkedList *piecesList;

};

typedef struct PossiblePieces_ PossiblePieces;

void setPossiblePieces(struct Game_ *, PossiblePieces *);

PossiblePieces *initPossiblePieces(struct Game_ *);

int getPossiblePiecesCount(PossiblePieces *);

Piece*getPieceWithId(int id);

void createDefaultPossiblePieces(struct Game_ * game, int size);

PossiblePieces *getPossiblePieces(struct Game_ * );

void addPossiblePiece(struct Game_ * , Piece *piece);

#endif //LABPROG_GAMEPIECES_H
