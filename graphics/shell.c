#include "shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void showPlayerTray(Player *player);

static int canPlace(int x, int y, BitMatrix *matrix) {

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

static Piece *sh_readPiece(Piece *prev) {

    if (prev != NULL) {
        printf("1) Repeat the previous piece\n");

        printf("2) Create a new piece\n");

        int choice;

        scanf("%d", &choice);

        switch (choice) {

            case 1:
                return prev;
            case 2:
                break;
            default:
                return sh_readPiece(prev);
        }
    }

    int pieceSize;

    printf("Insert the size of the piece you wish to create \n");

    scanf("%d", &pieceSize);

    char name[25];

    printf("Insert the name of the piece\n");

    scanf("%24s", name);

    BitMatrix *matrix = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, PIECE_MATRIX_WORD_SIZE);

    int pX, pY;

    int inserted = 0;

    do {

        printf ("Insert the X position of the next piece position\n");

        scanf("%d", &pX);

        printf("Insert the Y position of the next piece position\n");

        scanf("%d", &pY);

        if (canPlace(pX, pY, matrix) || inserted == 0) {
            m_setBit(matrix, pX, pY, 1);

            inserted++;
        } else {
            printf("Failed to add piece position. The position must be connected with another existing point!\n");
        }

    } while (inserted < pieceSize);

    return initPiece(pieceSize, name, matrix);
}

static void sh_readPieces(Game *game) {

    int pieceCount;

    printf("Insert the amount of pieces you want to have.\n");

    scanf("%d", &pieceCount);

    Piece *prev = NULL;

    for (int i = 0; i < pieceCount; i++) {

        addPossiblePiece(game, (prev = sh_readPiece(prev)));

    }

}

PossiblePieces *sh_readPossiblePieces(Game *game) {

    printf("1) Choose the default pieces for the game.\n");

    printf("2) Create your own pieces.\n");

    int choice;

    scanf("%d", &choice);

    switch (choice) {
        case 1: {
            createDefaultPossiblePieces(game, game->size);
            break;
        }
        case 2: {

            initPossiblePieces(game);

            sh_readPieces(game);

            break;
        }
        default:
            return sh_readPossiblePieces(game);
    }

    return game->p;
}

int sh_readGameSize() {
    system("clear");

    printf("Please insert the size of game you want to play. (Between %d and %d)\n", MIN_GAME_SIZE, MAX_GAME_SIZE);

    int gameSize;

    scanf("%d", &gameSize);

    if (gameSize < MIN_GAME_SIZE || gameSize > MAX_GAME_SIZE) {
        return sh_readGameSize();
    }

    return gameSize;
}

char *sh_readPlayerName() {
    system("clear");

    char *playerNameBuffer = malloc(sizeof(char) * MAX_PLAYER_NAME_SIZE);

    printf("Insert your name here: (Max of %d characters and no spaces)\n", MAX_PLAYER_NAME_SIZE);

    scanf("%23s", playerNameBuffer);

    return playerNameBuffer;
}

Position *sh_readPosition() {

    printf("Insert a position: \n");

    int x, y;

    printf("X: ");
    scanf("%d", &x);

    printf("Y: ");
    scanf("%d", &y);

    Position *pos = initPos(x, y);

    return pos;
}

void sh_alreadyPlayedThere() {

    printf("You have already played in this position!\n");

    sleep(2);
}

void sh_missed() {

    printf("Your shot has not hit any boats.\n");

    sleep(2);
}

void sh_hitBoat() {

    printf("Your shot has hit a boat!\n");

    sleep(2);

}

void sh_destroyedBoat() {
    printf("Your shot has destroyed a boat!\n");

    sleep(2);
}

void showPlayerTray(Player *player) {

    GameStorage *storage = pl_storage(player);

    int size = storage->size;

    printf("     ");

    for (int i = 0; i < size; i++) {
        if (size < 10) printf("    ");

        printf("%d ", i);

        if (size < 10) printf("   ");
    }

    printf("\n");

    for (int row = size - 1; row >= 0; row--) {

        for (int i = 0; i < size; i++) {
            printf("----");
        }

        printf("\n");

        if (row < 10) printf(" ");

        printf("%d ", row);

        for (int column = 0; column < size; column++) {

            Position p = {column, row};

            printf("|");

            char toPrint[4] = "  ";

            void *result = getAtPosition(storage, &p);

            if (result == NULL) {
                toPrint[1] = '.';
            } else {

                PointStorage *ps = (PointStorage *) result;

                if (ps->piece != NULL) {
                    if (ps->piece->destroyed == ps->piece->piece->size) {
                        toPrint[0] = '/';
                    } else {
                        toPrint[0] = '#';
                    }
                }

                if (ps->ownHitPoint != NULL) {
                    if (ps->ownHitPoint->hit) {
                        toPrint[1] = '~';
                    } else {
                        toPrint[1] = '+';
                    }
                }

                if (ps->opponentHitPoint != NULL) {
                    if (ps->opponentHitPoint->hit) {
                        toPrint[2] = '*';
                    } else {
                        toPrint[2] = '=';
                    }
                }
            }

            printf("%s", toPrint);
        }

        printf("|\n");
    }

    printf("# - Your boat, / - My destroyed boat, * - Opponent Hit, = - Opponent Miss, ~ - My Hit, + - My miss\n");
}

void sh_showPlaceablePieces(Player *game, PossiblePieces *piece, PieceInBoard **placed) {

    system("clear");

    printf("Insert your pieces in the desired position %s\n", pl_name(game));

    showPlayerTray(game);

    int pieceCount = getPossiblePiecesCount(piece);

    int placedPieces = 0;

    for (int i = 0; i < pieceCount; i++) {

        printf("%d) %s", i, getPieceWithId(piece, i)->name);

        if (placed[i] != NULL) {
            placedPieces++;
            printf(" - (Placed)\n");
        } else {
            printf("\n");
        }
    }

    if (placedPieces == pieceCount) {
        printf("-1) Move to next phase.\n");
    } else {
        printf("-1) Randomize remaining pieces.\n");
    }
}


void sh_waitingForOpponent() {
    printf("Waiting for your opponent...\n");
}

void sh_showNotPossibleToPlace(Piece *piece, Position *pos) {

    printf("You cannot place the piece %s in the postion (%d, %d).\n", piece->name, p_getBaseX(pos), p_getBaseY(pos));

    sleep(2);
}

void sh_showPiecePlaced(Piece *piece, Position *pos) {

    printf("The piece %s has been placed in the position (%d, %d)\n", piece->name, p_getBaseX(pos), p_getBaseY(pos));

    sleep(2);
}

void sh_displayGameSize(int size) {
    printf("The chosen game size is %d\n", size);
}

void sh_displayOpponentName(char *name) {
    printf("The opponents name is %s\n", name);
}

void sh_showAllPlaced() {

}

int sh_requestPieceToPlay(int maxSize) {

    int piece;

    scanf("%d", &piece);

    if (piece < -1 || piece > maxSize) {

        printf("That number is not valid\n");

        return sh_requestPieceToPlay(maxSize);
    }

    return piece;

}

PlacedDirection sh_readPlaceDirection() {

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

            return sh_readPlaceDirection();
    }

}

void sh_showYourTurn(Player *player) {
    showPlayerTray(player);

    printf("It's your turn to play, %s\n", pl_name(player));
    printf("Choose where you want to play!\n");
}

void sh_showOtherTurn(Player *player) {
    printf("It's %s's turn to play.\n", pl_name(player));

    printf("Wait for his move.\n");
}

void sh_otherPlayerDestroyedBoat(Player *player, Position *pos) {
    showPlayerTray(player);

    printf("The other player played in the position %d %d and destroyed one of your boats!\n",
            p_getBaseX(pos), p_getBaseY(pos));
}

void sh_otherPlayerHit(Player *player, Position *pos) {

    showPlayerTray(player);

    printf("The other player played in the position %d %d and hit one of your boats!\n",
           p_getBaseX(pos), p_getBaseY(pos));

}

void sh_otherPlayerMissed(Player *player, Position *pos) {

    showPlayerTray(player);

    printf("The other player played in the position %d %d and missed!\n",
           p_getBaseX(pos), p_getBaseY(pos));
}
