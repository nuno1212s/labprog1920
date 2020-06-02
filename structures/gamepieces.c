
#include <stdio.h>
#include <stdlib.h>

#include "gamepieces.h"
#include "pieces.h"
#include "game.h"

static void clearPossiblePieces(Game *game) {
    PossiblePieces *gamePossiblePieces = game->p;

    if (game->p->piecesList == NULL) {
        free(game->p);

        return;
    }

    ll_forEach(gamePossiblePieces->piecesList, (void (*)(void *)) gs_freePiece);

    ll_free(gamePossiblePieces->piecesList);

    free(gamePossiblePieces);
}

void setPossiblePieces(Game *game, PossiblePieces *pieces) {
    if (pieces != NULL) {
        clearPossiblePieces(game);
    }

    game->p = pieces;
}

PossiblePieces *initPossiblePieces(Game *g) {

    if (g->p != NULL) {
        clearPossiblePieces(g);
    }

    PossiblePieces *gamePossiblePieces = malloc(sizeof(PossiblePieces));

    gamePossiblePieces->piecesList = ll_initList();

    return (g->p = gamePossiblePieces);
}

Piece *getPieceWithId(PossiblePieces *pieces, int pieceID) {

    return ll_get(pieces->piecesList, pieceID);

}

PossiblePieces *getPossiblePieces(Game *game) {

    if (game->p == NULL) {
        initPossiblePieces(game);

        createDefaultPossiblePieces(game, 20);

        return game->p;

    }

    return game->p;
}

void createDefaultPossiblePieces(Game *game, int size) {

    initPossiblePieces(game);

    int pieceCount = (size * size) / 25;

    int onePiece = pieceCount * 0.1,
            twoPieces = pieceCount * 0.2,
            threePieces = pieceCount * 0.2,
            fourPiece = pieceCount * 0.2,
            lPiece = pieceCount * 0.2,
            weird = pieceCount * 0.1;

    BitMatrix *matrix = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, MATRIX_WORD_SIZE);

    m_setBit(matrix, 0, 0, 1);

    Piece *OneBlock = initPiece(1, "One block", matrix);

    for (int i = 0; i < onePiece; i++) {
        addPossiblePiece(game, OneBlock);
    }

    BitMatrix *twoBlock = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, MATRIX_WORD_SIZE);

    m_setBit(twoBlock, 0, 0, 1);
    m_setBit(twoBlock, 0, 1, 1);

    Piece *TwoBlock = initPiece(2, "Two blocks", twoBlock);

    for (int i = 0; i < twoPieces; i++) {
        addPossiblePiece(game, TwoBlock);
    }

    BitMatrix *threeBlock = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, MATRIX_WORD_SIZE);

    m_setBit(threeBlock, 0, 0, 1);
    m_setBit(threeBlock, 0, 1, 1);
    m_setBit(threeBlock, 0, 2, 1);

    Piece *ThreePiece = initPiece(3, "Three blocks", threeBlock);

    for (int i = 0; i < threePieces; i++) {
        addPossiblePiece(game, ThreePiece);
    }

    BitMatrix *destroyer = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, MATRIX_WORD_SIZE);

    m_setBit(destroyer, 0, 0, 1);
    m_setBit(destroyer, 0, 1, 1);
    m_setBit(destroyer, 0, 2, 1);
    m_setBit(destroyer, 0, 3, 1);

    Piece *fourBlock = initPiece(4, "Destroyer", destroyer);

    for (int i = 0; i < fourPiece; i++)
        addPossiblePiece(game, fourBlock);

    BitMatrix *lThing = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, MATRIX_WORD_SIZE);

    m_setBit(lThing, 0, 0, 1);
    m_setBit(lThing, 0, 1, 1);
    m_setBit(lThing, 0, 2, 1);
    m_setBit(lThing, 0, 3, 1);
    m_setBit(lThing, 1, 0, 1);
    m_setBit(lThing, 2, 0, 1);

    Piece *L = initPiece(6, "L", lThing);

    for (int i = 0; i < lPiece; i++)
        addPossiblePiece(game, L);

    BitMatrix *twoStripe = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, MATRIX_WORD_SIZE);

    m_setBit(twoStripe, 0, 0, 1);
    m_setBit(twoStripe, 0, 1, 1);
    m_setBit(twoStripe, 0, 2, 1);
    m_setBit(twoStripe, 0, 3, 1);
    m_setBit(twoStripe, 1, 0, 1);
    m_setBit(twoStripe, 1, 1, 1);
    m_setBit(twoStripe, 1, 2, 1);
    m_setBit(twoStripe, 1, 3, 1);
    m_setBit(twoStripe, 2, 0, 1);
    m_setBit(twoStripe, 3, 0, 1);

    Piece *weirdPiece = initPiece(10, "Weird", twoStripe);

    for (int i = 0; i < weird; i++)
        addPossiblePiece(game, weirdPiece);
}

int getPossiblePiecesCount(PossiblePieces *game) {
    return game == NULL ? 0 : ll_size(game->piecesList);
}

void addPossiblePiece(Game *game, Piece *piece) {
    ll_addLast(piece, getPossiblePieces(game)->piecesList);
}

void freePossiblePieces(Game *game) {

    PossiblePieces *pieces = g_possiblePieces(game);

    ll_forEach(pieces->piecesList, (void (*)(void *)) gs_freePiece);

    ll_free(pieces->piecesList);

    free(pieces);

}