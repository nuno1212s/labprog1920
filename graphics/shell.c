
#include "shell.h"
#include "../structures/game.h"
#include "../storagestructures/linkedlist.h"

#include <stdio.h>
#include <stdlib.h>

void sg_displayWhereToPlay(char *);

void sg_showOffline();

void sg_displayMenu() {

    printf("# Hello, Welcome to battle of ships.\n");
    printf("Please insert your player name (MAX 25 chars):\n");

    char name[26] = {'\0'};

    scanf("%25s", name);

    sg_displayWhereToPlay(name);
}

void sg_displayWhereToPlay(char *name) {

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
            sg_displayWhereToPlay(name);
            break;
        case 3:

            sg_showOffline(name);

            break;
        default:
            printf("# Number not unavailable.\n");

            sg_displayWhereToPlay(name);
            break;
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

    int chosen = 0;

    scanf("%d", &chosen);

    if (chosen < 0 || chosen >= maxPiece) {

        printf("This piece is not a valid number.\n");

        return choosePiece(maxPiece);
    }

    return chosen;
}

PlacedDirection requestPlacedDirection() {
    printf("Insert your desired rotation (0, 90, 180, 270)\n");

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

    if (placed[chosenPiece] != NULL) {
        removePlacedPiece(player->storage, placed[chosenPiece]);
    }

    Position *p = chooseWhereToPlace(traySize);

    PlacedDirection dir = requestPlacedDirection();

    PieceInBoard *placedPiece = insertPiece(player->storage, piece, p, dir);

    if (placedPiece == NULL) {

        printf("# Failed to place the piece in the request position. Maybe it goes outside the board, or there is already a piece there?\n");

        attemptPlacePiece(traySize, player, pieces, placed);

        return;
    }

    placed[chosenPiece] = placedPiece;

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

void placePieces(Game *g, Player *player, PossiblePieces *pieces) {

    PieceInBoard *placed[pieces->piecesList->size];

    while (placedCount(placed, pieces->piecesList->size) != pieces->piecesList->size) {
        printPieces(pieces, placed);

        attemptPlacePiece(g->size, player, pieces, placed);
    }

    printf("# You have placed all the pieces.\n");

    printf("# 1) Replace a piece.\n");

    printf("# 2) Finish.\n");

    int choice = 0;

    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printPieces(pieces, placed);
            attemptPlacePiece(g->size, player, pieces, placed);

            placePieces(g, player, pieces);
            break;
        case 2:
            return;
    }
}

void sg_showOffline(char *player1Name) {

    int traySize = readTraySize();

    PossiblePieces *pieces = readPieces(traySize);

    char player2Name[25] = {'\0'};

    scanName(2, player2Name);

    Player *p1 = initPlayer(player1Name, traySize), *p2 = initPlayer(player2Name, traySize);
    free(player2Name);

    Player **playerList = malloc(sizeof(Player *) * 2);

    playerList[0] = p1;
    playerList[1] = p2;

    Game *g = initGame(2, traySize, playerList);


}
