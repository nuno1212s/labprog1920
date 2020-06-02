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

/**
 * Set a game's possible pieces.
 */
void setPossiblePieces(struct Game_ *, PossiblePieces *);

/**
 * Instantiate a possible pieces structure
 * @return
 */
PossiblePieces *initPossiblePieces(struct Game_ *);

/**
 * Get the amount of possible pieces
 * @return
 */
int getPossiblePiecesCount(PossiblePieces *);

/**
 * Get the piece located in the id position of the possible pieces
 * @param id
 * @return
 */
Piece* getPieceWithId(PossiblePieces *, int id);

/**
 * Create the default game pieces for a given game
 * @param game
 * @param size
 */
void createDefaultPossiblePieces(struct Game_ * game, int size);

/**
 * Get the possible pieces of a game
 * @return
 */
PossiblePieces *getPossiblePieces(struct Game_ * );

/**
 * Add a possible piece to the game
 * @param piece
 */
void addPossiblePiece(struct Game_ * , Piece *piece);

void freePossiblePieces(struct Game_ *);

#endif //LABPROG_GAMEPIECES_H
