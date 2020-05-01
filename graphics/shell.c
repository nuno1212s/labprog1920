
#include "shell.h"

#include "../structures/gamestructures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static void printPiece(void *piece) {

    Piece *realPiece = (Piece *) piece;

    printf("Piece Name: %s\n Size: %d\n", realPiece->name, realPiece->size);

}

static void printPieceInBoard(void *piece) {

    PieceInBoard *piB = piece;

    int pieceSize = piB->piece->size;

    printf("# %s HP: %d/%d (%d hits taken)\n", piB->piece->name, pieceSize - piB->destroyed, pieceSize, piB->destroyed);

    Position **pos = calculateNewPositions(piB->piece, piB->basePos, piB->direction);

    for (int i = 0; i < pieceSize; i++) {
        printf("(%d, %d) ", p_getBaseX(pos[i]), p_getBaseY(pos[i]));
    }

    printf("\n");

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

static int readTraySize() {

    printf("# What size of tray do you want to play ?\n");

    int traySize = 0;

    scanf(" %d", &traySize);

    if (traySize < 20 || traySize > 40) {
        printf("# The tray size must be between 20 and 40\n");

        return readTraySize();
    }

    return traySize;
}

static PossiblePieces *readPieces(int traySize) {

    printf("# What pieces do you want to use?\n");

    printf("# 1) Default pieces.\n");

    printf("# 2) Create new pieces.\n");

    int res = 0;

    scanf("%d", &res);

    switch (res) {

        case 1:
            createDefaultPossiblePieces(traySize);
            return getPossiblePieces();

        case 2:
            return readPossiblePieces(traySize);

        default:
            return readPieces(traySize);

    }
}

int getNumberOfPiece(int traySize) {
    int numOfNewPieces = 0;
    int nMaxofNewPieces = (pow(traySize, 2)) / 25;

    printf("Number of new pieces to add: ");
    scanf("%d", &numOfNewPieces);

    if (numOfNewPieces < 1 || numOfNewPieces > nMaxofNewPieces) {
        printf("Number of new pieces must be between 1 and %d.\n", nMaxofNewPieces);
        return getNumberOfPiece(traySize);
    }

    return numOfNewPieces;
}

int getPieceSize(int maxSize) {
    int size;

    printf("New piece size: ");
    scanf("%d", &size);

    if (size <= 0 || size > 25) {
        printf("Piece size must be between 1 and %d.\n", maxSize);
        return getPieceSize(maxSize);
    }

    return size;
}

int canPlace(int x, int y, BitMatrix *matrix) {

    if (m_getBit(matrix, x, y) == 1) {
        return 0;
    }

    for (int aX = -1; aX <= 1; aX++) {
        for (int aY = -1; aY <= 1; aY++) {
            if (aX == aY) continue;

            int finalX = x + aX, finalY = y + aY;

            if (finalX < 0 || finalX >= matrix_cols(matrix)) {
                continue;
            }

            if (finalY < 0 || finalY >= matrix_rows(matrix)) {
                continue;
            }

            if (m_getBit(matrix, finalX, finalY)) {
                return 1;
            }

        }
    }

    return 0;
}

int getAmountofPiecesFor(int max) {

    printf("How many boats of this kind do you want to have? Max is %d\n", max);

    int amount = 0;

    scanf("%d", &amount);

    if (amount > max) {
        printf("The number is too high. The max is %d", max);
        return getAmountofPiecesFor(max);
    }

    return amount;
}

PossiblePieces *readPossiblePieces(int traySize) {

    initPossiblePieces();

    int numOfNewPieces = getNumberOfPiece(traySize);

    for (int i = 0; i < numOfNewPieces; i++) {
        char pieceName[24];
        int size, x, y;

        BitMatrix *matrix = createBitMatrix(5, 5, 1);

        printf("New piece name (Max of 24 characters): ");
        scanf("%24s", pieceName);

        size = getPieceSize(25);

        printf("Enter new piece´s body coordenates.\n");
        printf("X: ");
        scanf("%d", &x);
        printf("Y: ");
        scanf("%d", &y);
        m_setBit(matrix, x, y, 1);

        for (int j = 0; j < size - 1; j++) {
            printf("X: ");
            scanf("%d", &x);
            printf("Y: ");
            scanf("%d", &y);

            if (canPlace(x, y, matrix)) {
                m_setBit(matrix, x, y, 1);
            } else {
                printf("Failed to add piece. You cannot place it there.\n");
                j--;
                continue;
            }

            printBitMatrixReversed(matrix);
        }

        int amount = getAmountofPiecesFor((numOfNewPieces) - i);

        Piece *piece = initPiece(size, pieceName, matrix);

        for (int a = 0; a < amount; a++) {
            addPossiblePiece(piece);
        }

        i += (amount - 1);
    }

    return getPossiblePieces();

}

static void scanName(int playerID, char *name) {

    printf("# Player %d please insert your name (Max characters: %d):\n", playerID, 24);

    scanf("%24s", name);

}

static void printPieces(PossiblePieces *p, PieceInBoard **placedPieces) {

    LinkedList *pieceList = p->piecesList;

    struct Node_s *first = pieceList->first;

    int current = 0;

    printf("# -1 ) Randomize pieces not placed\n");

    while (first != NULL) {

        if (placedPieces[current] != NULL) {

            printf("# %d ) %s - Placed\n", current++, ((Piece *) first->data)->name);

        } else {

            printf("# %d ) %s \n", current++, ((Piece *) first->data)->name);

        }

        first = first->next;
    }
}

static int choosePiece(int maxPiece) {

    printf("Choose the piece to place:\n");

    int chosen = 0;

    scanf("%d", &chosen);

    if (chosen != -1 && (chosen < 0 || chosen >= maxPiece)) {

        printf("This piece is not a valid number.\n");

        return choosePiece(maxPiece);
    }

    return chosen;
}

static PlacedDirection requestPlacedDirection() {
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

static Position *chooseWhereToPlace(int traySize) {
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

static void attemptPlacePiece(int traySize, Player *player, PossiblePieces *pieces, PieceInBoard **placed) {
    int chosenPiece = choosePiece(pieces->piecesList->size);

    if (chosenPiece == -1) {
        printf("Randomizing...");

        randomizePiecesLeft(player, traySize, placed, pieces);

        displayGameTray(player, traySize);

        return;
    }

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

    PieceInBoard *placedPiece = addPieceChosen(player, p, piece, dir);

    if (placedPiece == NULL) {

        printf("# Failed to place the piece in the request position. Maybe it goes outside the board, or there is already a piece there?\n");

        attemptPlacePiece(traySize, player, pieces, placed);

        return;
    }

    placed[chosenPiece] = placedPiece;

    displayGameTray(player, traySize);
}

static int placedCount(PieceInBoard **board, int pieces) {
    int count = 0;

    for (int i = 0; i < pieces; i++) {
        if (board[i] != NULL) {
            count++;
        }
    }

    return count;
}

static void placedAllPieces(Game *g, Player *player, PossiblePieces *pieces, PieceInBoard **placed) {
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

static void placePieces(Game *g, Player *player, PossiblePieces *pieces) {

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

    void *(*get)(void *, Position *) = NULL;

    void *storageP = NULL;

    switch (storage->type) {
        case GS_QUAD:
            get = (void *(*)(void *, Position *)) qt_lookup;
            storageP = storage->data.quadTree;
            break;
        case GS_MATRIX:
            get = (void *(*)(void *, Position *)) m_lookup;
            storageP = storage->data.matrix;
            break;
    }


    for (int i = 0; i < size; i++) {
        printf("%d ", i);
    }

    printf("\n");

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

                if (ps->piece != NULL) {
                    printf("B");
                }

                if (ps->ownHitPoint != NULL) {
                    if (ps->ownHitPoint->hit) {
                        printf("MH");
                    } else {
                        printf("MM");
                    }
                }

                if (ps->opponentHitPoint != NULL) {
                    if (ps->opponentHitPoint->hit) {
                        printf("OH");
                    } else {
                        printf("OM");
                    }
                }
            }
        }

        printf("|\n");
    }

    printf("B - Your boat, OH - Opponent Hit, OM - Opponent Miss, MH - My Hit, MM - My miss\n");
}

static void getPlaySpot(Game *g, Player *player) {
    printf("Choose the place to play.\n");

    Position *pos = chooseWhereToPlace(g->size);

    Hit h = playAt(g, player, pos);

    if (h.hitType == H_ALREADY_HIT) {
        printf("You have already played here, please choose another position.\n");

        getPlaySpot(g, player);

        return;
    } else if (h.hitType == H_HIT_BOAT) {
        printf("You have hit a boat!\n");

        return;
    } else if (h.hitType == H_DESTROYED_BOAT) {
        printf("You have destroyed a boat!\n");
    } else if (h.hitType == H_MISSED) {
        printf("Your shot has not hit any boats.\n");
    }
}

void displayGameForPlayer(Game *g, Player *player) {

    printf("It's your turn %s\n", player->name);

    printf("You still have the following boats:\n");

    ll_forEach(player->currentActivePieces, printPieceInBoard);

    printf("This is your game tray:\n");

    displayGameTray(player, g->size);

    getPlaySpot(g, player);
}

void sg_displayGame(Game *g) {

    int winner = 0;

    while ((winner = hasFinished(g)) == -1) {
        displayGameForPlayer(g, getCurrentPlayer(g));
    }

    Player *p = g->players[winner];

    printf("The winner of the game was: %s with %d boats remaining\n", p->name, ll_size(p->currentActivePieces));

}