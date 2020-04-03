
#include "shell.h"
#include "../structures/game.h"
#include "../storagestructures/linkedlist.h"
#include "../structures/gamestructures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printPiece(void *piece) {

    Piece *realPiece = (Piece *) piece;

    printf("Piece Name: %s\n Size: %d\n", realPiece->name, realPiece->size);

}

Game *sg_displayWhereToPlay(char *);

Game *sg_showOffline();

Game *sg_displayMenu() {

    printf("# Hello, Welcome to battle of ships.\n");
    printf("Please insert your player name (MAX 25 chars):\n");

    char name[26] = {'\0'};

    scanf("%25s", name);

    return sg_displayWhereToPlay(name);
}

Game *sg_displayWhereToPlay(char *name) {

    printf("# Choose where you want to play: \n");

    printf("# 1) Online (Unavailable)\n");

    printf("# 2) Offline, 2 Shells (Unavailable)\n");

    printf("# 3) Offline, 1 Shell\n");

    int choice = 0;

    scanf("%d", &choice);

    switch (choice) {

        case 1:
        case 2:
            printf("# This is unavailable at the time.\n");
            return sg_displayWhereToPlay(name);
        case 3:

            return sg_showOffline(name);
        default:
            printf("# Number not unavailable.\n");

            return sg_displayWhereToPlay(name);
    }
}

int readTraySize() {

    printf("# What size of tray do you want to play ?\n");

    int traySize = 0;

    scanf(" %d", &traySize);

    if (traySize < 20 || traySize > 40) {
        printf("# The tray size must be between 20 and 40\n");

        return readTraySize();
    }

    return traySize;
}

PossiblePieces *readPieces(int traySize) {

    printf("# What pieces do you want to use?\n");

    printf("# 1) Default pieces.\n");

    printf("# 2) Create new pieces.\n");

    int res = 0;

    scanf("%d", &res);

    switch (res) {

        case 1:
            return getPossiblePieces();

        default:
            return readPieces(traySize);

    }
}

void scanName(int playerID, char *name) {

    printf("# Player %d please insert your name (Max characters: %d):\n", playerID, 24);

    scanf("%24s", name);

}

void printPieces(PossiblePieces *p, PieceInBoard **placedPieces) {

    LinkedList *pieceList = p->piecesList;

    struct Node_s *first = pieceList->first;

    int current = 0;

    while (first != NULL) {

        if (placedPieces[current] != NULL) {

            printf("# %d ) %s - Placed\n", current++, ((Piece *) first->data)->name);

        } else {

            printf("# %d ) %s \n", current++, ((Piece *) first->data)->name);

        }

        first = first->next;
    }
}

int choosePiece(int maxPiece) {

    printf("Choose the piece to place:\n");

    int chosen = 0;

    scanf("%d", &chosen);

    if (chosen < 0 || chosen >= maxPiece) {

        printf("This piece is not a valid number.\n");

        return choosePiece(maxPiece);
    }

    return chosen;
}

PlacedDirection requestPlacedDirection() {
    printf("Insert your desired rotation (0, 90, 180, 270):\n");

    int rotation = 0;

    scanf("%d", &rotation);

    switch (rotation) {
        case 0:
            return P_UP;
        case 90:
            return P_LEFT;
        case 180:
            return P_DOWN;
        case 270:
            return P_RIGHT;
        default:
            printf("That is not a valid rotation\n");

            return requestPlacedDirection();
    }
}

Position *chooseWhereToPlace(int traySize) {
    printf("Choose where to place: \n");

    int x = 0, y = 0;

    printf("X:\n");

    scanf("%d", &x);

    printf("Y:\n");

    scanf("%d", &y);

    if (x < 0 || y < 0 || x > traySize || y > traySize) {
        printf("Choose a number between %d and %d\n", 0, traySize);

        return chooseWhereToPlace(traySize);
    }

    return initPos(x, y);
}

void attemptPlacePiece(int traySize, Player *player, PossiblePieces *pieces, PieceInBoard **placed) {
    int chosenPiece = choosePiece(pieces->piecesList->size);

    Piece *piece = ll_get(pieces->piecesList, chosenPiece);

    if (piece == NULL) {
        printf("# Failed.\n");

        attemptPlacePiece(traySize, player, pieces, placed);

        return;
    }

    printPiece(piece);

    if (placed[chosenPiece] != NULL) {
        removePlacedPiece(player->storage, placed[chosenPiece]);
    }

    Position *p = chooseWhereToPlace(traySize);

    PlacedDirection dir = requestPlacedDirection();

    printf("%d", player->storage->type);

    PieceInBoard *placedPiece = insertPiece(player->storage, piece, p, dir);

    if (placedPiece == NULL) {

        printf("# Failed to place the piece in the request position. Maybe it goes outside the board, or there is already a piece there?\n");

        attemptPlacePiece(traySize, player, pieces, placed);

        return;
    }

    placed[chosenPiece] = placedPiece;

    displayGameTray(player, traySize);
}

int placedCount(PieceInBoard **board, int pieces) {
    int count = 0;

    for (int i = 0; i < pieces; i++) {
        if (board[i] != NULL) {
            count++;
        }
    }

    return count;
}

void placedAllPieces(Game *g, Player *player, PossiblePieces *pieces, PieceInBoard **placed) {
    printf("# You have placed all the pieces.\n");

    printf("# 1) Replace a piece.\n");

    printf("# 2) Finish.\n");

    int choice = 0;

    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printPieces(pieces, placed);
            attemptPlacePiece(g->size, player, pieces, placed);

            placedAllPieces(g, player, pieces, placed);
            break;
        case 2:
            return;
        default:
            printf("Error, number not expected");
            placedAllPieces(g, player, pieces, placed);

            break;
    }
}

void placePieces(Game *g, Player *player, PossiblePieces *pieces) {

    PieceInBoard *placed[pieces->piecesList->size];

    memset(placed, 0, pieces->piecesList->size * sizeof(PieceInBoard *));

    while (placedCount(placed, pieces->piecesList->size) != pieces->piecesList->size) {
        printPieces(pieces, placed);

        attemptPlacePiece(g->size, player, pieces, placed);
    }

    placedAllPieces(g, player, pieces, placed);

}

Game *sg_showOffline(char *player1Name) {

    int traySize = readTraySize();

    PossiblePieces *pieces = readPieces(traySize);

    LinkedList *list = pieces->piecesList;

    ll_print(list, printPiece);

    char player2Name[25] = {'\0'};

    scanName(2, player2Name);

    Player *p1 = initPlayer(player1Name, traySize), *p2 = initPlayer(player2Name, traySize);

    Player **playerList = malloc(sizeof(Player *) * 2);

    playerList[0] = p1;
    playerList[1] = p2;

    Game *g = initGame(2, traySize, playerList);

    printf("Player 1 please place your pieces.\n");

    placePieces(g, p1, pieces);

    printf("Player 2 please place your pieces.\n");

    placePieces(g, p2, pieces);

    return g;
}

void displayGameTray(Player *player, int size) {

    GameStorage *storage = player->storage;

    void* (*get)(void *, Position *) = NULL;

    void *storageP = NULL;

    switch (storage->type) {
        case GS_QUAD:
            get = (void* (*)(void *, Position *)) qt_lookup;
            storageP = storage->data.quadTree;
            break;
        case GS_MATRIX:
            get = (void* (*)(void *, Position *)) m_lookup;
            storageP = storage->data.matrix;
            break;
    }

    for (int row = size; row >= 0; row--) {


        for (int i = 0; i < size; i++) {
            printf("--");
        }

        printf("\n");

        for (int column = 0; column < size; column++) {

            Position p = {column, row};

            printf("|");

            void *result = get(storageP, &p);

            if (result == NULL) {
                printf(".");
            } else {

                PointStorage *ps = (PointStorage *) result;

                if (ps->piece != NULL && ps->opponentHitPoint != NULL) {

                    printf("H");

                } else if (ps->piece != NULL) {
                    printf("B");
                } else {
                    printf("M");
                }

            }

        }

        printf("|\n");

    }

}