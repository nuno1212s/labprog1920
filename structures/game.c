#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../storagestructures/bitmap.h"
#include "gamestructures.h"

#define MATRIX_THRESHOLD 10

static PossiblePieces *possiblePieces = NULL;

PossiblePieces *initPossiblePieces() {

    possiblePieces = malloc(sizeof(PossiblePieces));

    possiblePieces->piecesList = ll_initList();

    return possiblePieces;
}

void createDefaultPossiblePieces();

PossiblePieces *getPossiblePieces() {

    if (possiblePieces == NULL) {
        initPossiblePieces();

        createDefaultPossiblePieces();

        return possiblePieces;

    }

    return possiblePieces;
}

void createDefaultPossiblePieces() {

    BitMatrix *matrix = createBitMatrix(5, 5, 1);

    m_setBit(matrix, 0, 0, 1);

    addPossiblePiece( initPiece(1, "One block wonder", matrix));

    BitMatrix *twoBlock = createBitMatrix(5, 5, 1);

    m_setBit(twoBlock, 0, 0, 1);
    m_setBit(twoBlock, 0, 1, 1);

    addPossiblePiece(initPiece(2, "Two block machine", twoBlock));

    BitMatrix *threeBlock = createBitMatrix(5, 5, 1);

    m_setBit(threeBlock, 0, 0, 1);
    m_setBit(threeBlock, 0, 1, 1);
    m_setBit(threeBlock, 0, 2, 1);

    addPossiblePiece(initPiece(3, "Three block thingy", threeBlock));

    BitMatrix *destroyer = createBitMatrix(5, 5, 1);

    m_setBit(destroyer, 0, 0, 1);
    m_setBit(destroyer, 0, 1, 1);
    m_setBit(destroyer, 0, 2, 1);
    m_setBit(destroyer, 0, 3, 1);

    addPossiblePiece(initPiece(4, "Destroyer", destroyer));

    BitMatrix *lThing = createBitMatrix(5, 5, 1);

    m_setBit(lThing, 0, 0, 1);
    m_setBit(lThing, 0, 1, 1);
    m_setBit(lThing, 0, 2, 1);
    m_setBit(lThing, 0, 3, 1);
    m_setBit(lThing, 1, 0, 1);
    m_setBit(lThing, 2, 0, 1);

    addPossiblePiece(initPiece(6, "L Remover", lThing));
}

int getPossiblePiece() {
    return possiblePieces == NULL ? 0 : ll_size(possiblePieces->piecesList);
}

void addPossiblePiece(Piece *piece) {

    ll_addLast(piece, getPossiblePieces()->piecesList);

}

Game *initGame(int players, int size, Player **player) {

    Game *game = malloc(sizeof(Game));

    game->gameID = rand() % 2000000000;

    game->playerCount = players;
    game->size = size;

    game->players = player;

    game->currentPlayerIndex = rand() % players;

    return game;
}

void freeGame(Game *game) {

    for (int i = 0; i < game->playerCount; i++) {

        Player *player = game->players[i];

        freePlayer(player);

    }

    free(game->players);

    free(game);
}

SearchingForGame *initSearchGame(PossiblePieces *pieces, int size, Player *player) {

    SearchingForGame *game = malloc(sizeof(SearchingForGame));

    game->size = size;

    game->pieces = pieces;
    game->player = player;

    return game;
}

void deleteSearchGame(SearchingForGame *game) {

    free(game);

}

static Hit destroyed(PieceInBoard *piece) {
    Hit h;

    h.hitType = H_DESTROYED_BOAT;

    h.hit = piece;

    return h;
}

static Hit hit(PieceInBoard *piece) {

    Hit h;

    h.hitType = H_HIT_BOAT;
    h.hit = piece;

    return h;
}

static Hit missed() {
    Hit h;

    h.hitType = H_MISSED;

    h.hit = NULL;

    return h;
}

static Hit alreadyHit() {
    Hit h;

    h.hitType = H_ALREADY_HIT;
    h.hit = NULL;

    return h;
}

Player *getCurrentPlayer(Game *game) {

    if (game->currentPlayerIndex >= game->playerCount) {
        return NULL;
    }

    Player *p = game->players[game->currentPlayerIndex];

    return p;
}

void goToNextPlayer(Game *game) {

    game->currentPlayerIndex++;

    if (game->currentPlayerIndex >= game->playerCount) {
        game->currentPlayerIndex = 0;
    }

}

Hit playAt(Game *g, Player *player, Position *pos) {

    if (hasPlayed(player->storage, pos)) {
        return alreadyHit();
    }

    HitResponse res;

    Player *otherPlayer = NULL;

    for (int i = 0; i < g->playerCount; i++) {

        if (i == g->currentPlayerIndex) continue;

        Player *p = g->players[i];

        res = attemptHit(p->storage, pos);
        otherPlayer = p;
    }

    if (otherPlayer == NULL) {
        fprintf(stderr, "There was an error with the player");
        exit(1);
    }

    if (res.hit_type == HR_HIT_BOAT) {
        registerHit(player->storage, pos, 1);

        if (hasBeenDestroyed(otherPlayer->storage, res.hit)) {

            int index = ll_indexOf(otherPlayer->currentActivePieces, res.hit);

            ll_remove(otherPlayer->currentActivePieces, index);

            return destroyed(res.hit);
        }

        return hit(res.hit);
    } else {
        registerHit(player->storage, pos, 0);

        return missed();
    }

}

int hasFinished(Game *g) {

    int playerThatLost = -1;

    for (int i = 0; i < g->playerCount; i++) {

        Player *p = g->players[i];

        if (ll_size(p->currentActivePieces) == 0) {
            //If the player has no active pieces, then the other player has won

            playerThatLost = i;
            break;
        }

    }

    if (playerThatLost == 0) return 1;
    else if (playerThatLost == 1) return 0;

    return -1;
}

Player *initPlayer(char *name, int size) {

    Player *player = malloc(sizeof(Player));

    player->name = strdup(name);

    player->storage = initGameStorage(size, size > MATRIX_THRESHOLD ? GS_QUAD : GS_MATRIX);

    return player;
}

PieceInBoard* addPieceChosen(Player *player, Position *position, Piece *piece, PlacedDirection dir) {

    GameStorage *storage = player->storage;

    if (canPlacePiece(player, position, piece, dir)) {

        PieceInBoard *board = insertPiece(storage, piece, position, dir);

        if (board == NULL) {
            ll_addLast(board, player->currentActivePieces);
        }

        return board;
    } else {
        return NULL;
    }

}

int canPlacePiece(Player *player, Position *pos, Piece *piece, PlacedDirection dir) {

    GameStorage *storage = player->storage;

    return gs_canPlayPiece(storage, piece, pos, dir);
}

int getRand(int size) {

    return rand() % size;

}

SearchingForGame *initGameForPlayer(Player *player, int size) {

    return initSearchGame(getPossiblePieces(), size, player);

}

SearchingForGame *randomizePieces(Player *player, int size) {

    GameStorage *storage = player->storage;

    PossiblePieces *pieces = getPossiblePieces();

    struct Node_s *first = pieces->piecesList->first;

    while (first != NULL) {

        Piece *piece = (Piece *) first->data;

        int x = getRand(size), y = getRand(size);

        PlacedDirection dir = getRand(4);

        Position pos = {x, y};

        if (insertPiece(storage, piece, &pos, dir) == NULL) {
            //Retry a different position
            continue;
        }

        first = first->next;
    }

    return initSearchGame(pieces, size, player);
}

void freePlayer(Player *player) {

    freeGameStorage(player->storage);

    free(player->name);

    free(player);
}