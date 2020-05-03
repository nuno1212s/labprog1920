#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../storagestructures/bitmap.h"
#include "gamestructures.h"

#define MATRIX_THRESHOLD 10

static PossiblePieces *possiblePieces = NULL;

PossiblePieces *initPossiblePieces() {

    if (possiblePieces != NULL) {
        ll_forEach(possiblePieces->piecesList, (void (*)(void *)) gs_freePiece);

        ll_free(possiblePieces->piecesList);

        free(possiblePieces);
    }

    possiblePieces = malloc(sizeof(PossiblePieces));

    possiblePieces->piecesList = ll_initList();

    return possiblePieces;
}

PossiblePieces *getPossiblePieces() {

    if (possiblePieces == NULL) {
        initPossiblePieces();

        createDefaultPossiblePieces(20);

        return possiblePieces;

    }

    return possiblePieces;
}

void createDefaultPossiblePieces(int size) {

    initPossiblePieces();

    int pieceCount = (size * size) / 25;

    int onePiece = pieceCount * 0.1,
            twoPieces = pieceCount * 0.2,
            threePieces = pieceCount * 0.2,
            fourPiece = pieceCount * 0.2,
            lPiece = pieceCount * 0.2,
            weird = pieceCount * 0.1;

    BitMatrix *matrix = createBitMatrix(5, 5, 1);

    m_setBit(matrix, 0, 0, 1);

    Piece *OneBlock = initPiece(1, "One block", matrix);

    for (int i = 0; i < onePiece; i++) {
        addPossiblePiece(OneBlock);
    }

    BitMatrix *twoBlock = createBitMatrix(5, 5, 1);

    m_setBit(twoBlock, 0, 0, 1);
    m_setBit(twoBlock, 0, 1, 1);

    Piece *TwoBlock = initPiece(2, "Two blocks", twoBlock);

    for (int i = 0; i < twoPieces; i++) {
        addPossiblePiece(TwoBlock);
    }

    BitMatrix *threeBlock = createBitMatrix(5, 5, 1);

    m_setBit(threeBlock, 0, 0, 1);
    m_setBit(threeBlock, 0, 1, 1);
    m_setBit(threeBlock, 0, 2, 1);

    Piece *ThreePiece = initPiece(3, "Three blocks", threeBlock);

    for (int i = 0; i < threePieces; i++) {
        addPossiblePiece(ThreePiece);
    }

    BitMatrix *destroyer = createBitMatrix(5, 5, 1);

    m_setBit(destroyer, 0, 0, 1);
    m_setBit(destroyer, 0, 1, 1);
    m_setBit(destroyer, 0, 2, 1);
    m_setBit(destroyer, 0, 3, 1);

    Piece *fourBlock = initPiece(4, "Destroyer", destroyer);

    for (int i = 0; i < fourPiece; i++)
        addPossiblePiece(fourBlock);

    BitMatrix *lThing = createBitMatrix(5, 5, 1);

    m_setBit(lThing, 0, 0, 1);
    m_setBit(lThing, 0, 1, 1);
    m_setBit(lThing, 0, 2, 1);
    m_setBit(lThing, 0, 3, 1);
    m_setBit(lThing, 1, 0, 1);
    m_setBit(lThing, 2, 0, 1);

    Piece *L = initPiece(6, "L", lThing);

    for (int i = 0; i < lPiece; i++)
        addPossiblePiece(L);

    BitMatrix *twoStripe = createBitMatrix(5, 5, 1);

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

    for (int i = 0; i < weird; i ++)
        addPossiblePiece(weirdPiece);
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

        goToNextPlayer(g);

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
//(13, 1) (12, 1) (11, 1)

Player *initPlayer(char *name, int size) {

    Player *player = malloc(sizeof(Player));

    player->name = strdup(name);

    player->storage = initGameStorage(size, size > MATRIX_THRESHOLD ? GS_QUAD : GS_MATRIX);

    player->currentActivePieces = ll_initList();

    return player;
}

PieceInBoard *addPieceChosen(Player *player, Position *position, Piece *piece, PlacedDirection dir) {

    GameStorage *storage = player->storage;

    if (canPlacePiece(player, position, piece, dir)) {

        PieceInBoard *board = insertPiece(storage, piece, position, dir);

        if (board != NULL) {
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

void randomizePiecesLeft(Player *player, int size, PieceInBoard **placed, PossiblePieces *pieces) {

    LinkedList *list = pieces->piecesList;

    struct Node_s *first = list->first;

    int current = 0;

    while (first != NULL) {

        if (placed != NULL && placed[current] != NULL) {
            current++;
            continue;
        }

        Piece *piece = (Piece *) first->data;

        int x = getRand(size), y = getRand(size);

        PlacedDirection dir = getRand(4);

        Position *pos = initPos(x, y);

        PieceInBoard *placedPiece = NULL;

        if ((placedPiece = addPieceChosen(player, pos, piece, dir)) == NULL) {
            //Retry a different position

            p_free(pos);

            continue;
        }

        if (placed != NULL) {
            placed[current] = placedPiece;
        }

        current++;
        first = first->next;
    }

}

SearchingForGame *randomizePieces(Player *player, int size) {

    randomizePiecesLeft(player, size, NULL, getPossiblePieces());

    return initSearchGame(getPossiblePieces(), size, player);
}

void freePlayer(Player *player) {

    freeGameStorage(player->storage);

    free(player->name);

    free(player);
}