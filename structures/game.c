#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "gamestructures.h"

#define MATRIX_THRESHOLD 10

Game *initGame(int players, int size, Player **player) {

    Game *game = malloc(sizeof(Game));

    game->gameID = rand() % 2000000000;

    game->playerCount = players;
    game->size = size;

    game->players = player;

    game->currentPlayerIndex = rand() % players;

    game->p = NULL;

    return game;
}

void freeGame(Game *game) {

    for (int i = 0; i < game->playerCount; i++) {

        Player *player = game->players[i];

        freePlayer(player);

    }

    freePossiblePieces(game);

    free(game->players);

    free(game);
}

static Hit destroyed() {
    Hit h;

    h.hitType = H_DESTROYED_BOAT;

    return h;
}

static Hit hit() {

    Hit h;

    h.hitType = H_HIT_BOAT;

    return h;
}

static Hit missed() {
    Hit h;

    h.hitType = H_MISSED;

    return h;
}

static Hit alreadyHit() {
    Hit h;

    h.hitType = H_ALREADY_HIT;

    return h;
}

Player *getPlayer(Game *game, int index) {
    return game->players[index];
}

Player *getCurrentPlayer(Game *game) {

    if (game->currentPlayerIndex >= game->playerCount) {
        return NULL;
    }

    Player *p = game->players[game->currentPlayerIndex];

    return p;
}

Player *getOtherPlayer(Game *game) {
    if (game->currentPlayerIndex == 1) {
        return game->players[0];
    } else if (game->currentPlayerIndex == 0) {
        return game->players[1];
    }
}

void goToNextPlayer(Game *game) {

    game->currentPlayerIndex = (++game->currentPlayerIndex % game->playerCount);
}

int hasPlayedAt(Player *player, Position *pos) {

    return hasPlayed(pl_storage(player), pos);

}

Hit playAt(Game *g, Player *player, Position *pos) {

    HitResponse res;

    Player *otherPlayer = NULL;

    for (int i = 0; i < g->playerCount; i++) {

        if (i == g->currentPlayerIndex) continue;

        Player *p = g->players[i];

        res = attemptHit(pl_storage(p), pos);

        otherPlayer = p;
    }

    if (otherPlayer == NULL) {
        fprintf(stderr, "There was an error with the player");
        exit(1);
    }

    if (res.hit_type == HR_HIT_BOAT) {
        if (hasBeenDestroyed(pl_storage(otherPlayer), res.hit)) {
            pl_setActivePieces(otherPlayer, pl_activePieces(otherPlayer) - 1);
            return destroyed();
        }

        return hit();
    } else {
        goToNextPlayer(g);

        return missed();
    }

}

void registerPlayResult(Game *game, Player *player, Position *position, HitType type) {

    int result;

    if (type == H_DESTROYED_BOAT) {
        Player *otherPlayer = NULL;

        for (int i = 0; i < game->playerCount; i++) {

            if (i == game->currentPlayerIndex) continue;

            otherPlayer = game->players[i];
        }

        pl_setActivePieces(otherPlayer, pl_activePieces(otherPlayer) - 1);
    }

    switch (type) {
        case H_HIT_BOAT:
        case H_DESTROYED_BOAT:
            result = 1;
            break;
        default:
            result = 0;

            goToNextPlayer(game);
            break;
    }

    registerHit(pl_storage(player), position, result);

}

int hasFinished(Game *g) {

    int playerThatLost = -1;

    for (int i = 0; i < g->playerCount; i++) {

        Player *p = g->players[i];

        if (pl_activePieces(p) == 0) {
            //If the player has no active pieces, then the other player has won

            playerThatLost = i;
            break;
        }

    }

    //We get the player that has lost, we want to return the other one
    if (playerThatLost == 0) return 1;
    else if (playerThatLost == 1) return 0;

    return -1;
}
//(13, 1) (12, 1) (11, 1)

Player *initPlayer(char *name, int size, int isLocal) {

    Player *player = malloc(sizeof(Player));

    if (name != NULL) {
        player->name = strdup(name);
    } else {
        player->name = NULL;
    }

    if (isLocal) {
        player->storage = initGameStorage(size);
    } else {
        player->storage = NULL;
    }

    player->currentActivePieceCount = 0;

    return player;
}

PieceInBoard *addPieceChosen(Player *player, Position *position, Piece *piece, PlacedDirection dir) {

    GameStorage *storage = pl_storage(player);

    if (canPlacePiece(player, position, piece, dir)) {

        PieceInBoard *board = insertPiece(storage, piece, position, dir);

        pl_setActivePieces(player, pl_activePieces(player) + 1);

        return board;
    } else {
        return NULL;
    }

}

int canPlacePiece(Player *player, Position *pos, Piece *piece, PlacedDirection dir) {

    GameStorage *storage = pl_storage(player);

    return gs_canPlayPiece(storage, piece, pos, dir);
}

int getRand(int size) {

    return rand() % size;

}

void randomizePiecesLeft(Player *player, int size, PieceInBoard **placed, PossiblePieces *pieces) {

    srand(time(0));

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

        printf("Attempting to place piece %s in %d %d \n", piece->name, p_getBaseX(pos), p_getBaseY(pos));

        PieceInBoard *placedPiece = NULL;

        if ((placedPiece = addPieceChosen(player, pos, piece, dir)) == NULL) {
            //Retry a different position

            printf("failed\n");

            p_free(pos);

            continue;
        }

        if (placed != NULL) {
            placed[current] = placedPiece;
        }

        p_free(pos);

        current++;
        first = first->next;
    }

}

void freePlayer(Player *player) {

    if (pl_storage(player) != NULL)
        freeGameStorage(pl_storage(player));

    free(player->name);

    free(player);
}