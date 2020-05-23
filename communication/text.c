#include "text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sem.h"
#include <unistd.h>
#include "../storagestructures/bitmap.h"
#include "../storagestructures/linkedlist.h"

#define TEXT_FILE "gameinfo.txt"

#define BUFFER_SIZE 1024

static FILE *fp;

static int isBlocking = 0;

static char BUFFER[BUFFER_SIZE];

static void txt_repeatPiece();

static void txt_writePiece(Piece *);

static Piece *readPiece(Piece *);

static BitMatrix *readMatrix();

static void initFP(int host) {
    if (host) {
        //If we are the host, reset the file.
        fp = fopen(TEXT_FILE, "w+");
    } else {
        //If we are not the host, just open the file as it is.
        fp = fopen(TEXT_FILE, "r+");
    }

    if (fp == NULL) {
        fprintf(stderr, "Failed to open the text file %s\n", TEXT_FILE);

        exit(1);
    }
}

void failAndWait() {

    //Release the semaphore so that others can do their work
    semPost();

    perror("FAILED TO READ FROM FILE... WAITING 3 SECONDS FOR OTHER TERMINAL...");

    sleep(3);

}

void txt_flush() {
    fflush(fp);
}

void txt_init(int host) {
    initFP(host);

    initSem(host);
}

void txt_semWait() {

    printf("TXT_SEMWAIT CALLED SEM RESULT: %d, BLOCKING = %d\n", semValue(), isBlocking);

    if (isBlocking) {
        isBlocking = 0;
    } else {
        semWait();

        printf("TXT_SEMWAIT RELEASED\n");
    }
}

void txt_block() {
    //The semaphore is at 1 when no one is accessing it. We want to set it at 0 so that others cannot access it when we are doing something that they have to wait for

    printf("TXT_BLOCK CALLED, CURRENT SEM RESULT: %d\n", semValue());

    semWait();

    printf("TXTBLOCK RELEASED\n");

    isBlocking = 1;
}

int txt_readGameSize() {
    txt_semWait();

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        failAndWait();

        return txt_readGameSize();
    }

    int gameSize = (int) strtol(BUFFER, NULL, 10);

    semPost();

    return gameSize;
}

void txt_sendGameSize(int gameSize) {

    txt_semWait();

    fprintf(fp, "%d\n", gameSize);

    txt_flush();

    semPost();

}

void txt_writePossiblePieces(PossiblePieces *pieces) {

    txt_semWait();

    fprintf(fp, "%d\n", ll_size(pieces->piecesList));

    struct Node_s *node = pieces->piecesList->first;

    Piece *prevPiece = NULL,
            *currentPiece = NULL;

    while (node != NULL) {
        currentPiece = node->data;

        if (prevPiece == currentPiece) {
            txt_repeatPiece();
        } else {
            txt_writePiece(currentPiece);
        }

        prevPiece = currentPiece;
        node = node->next;
    }

    txt_flush();

    semPost();
}

void txt_sendPlayerInformation(Player *player) {

    txt_semWait();

    fprintf(fp, "%s\n", player->name);

    txt_flush();

    semPost();

}

void txt_readPlayerInformation(Player *player) {

    txt_semWait();

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {

        failAndWait();

        txt_readPlayerInformation(player);

        return;
    }

    player->name = strdup(BUFFER);

    semPost();
}

static void txt_writePiece(Piece *piece) {
    fprintf(fp, "PIECE\n%s-%d\n", piece->name, piece->size);

    for (int row = 0; row < matrix_rows(piece->matrix); row++) {

        for (int col = 0; col < matrix_cols(piece->matrix); col++) {

            fprintf(fp, "%d ", m_getBit(piece->matrix, row, col));

        }
    }

    fprintf(fp, "\n");

}

static void txt_repeatPiece() {
    fprintf(fp, "REPEAT\n");
}

PossiblePieces *txt_readPossiblePieces(Game *game) {

    txt_semWait();

    PossiblePieces *pieces = initPossiblePieces(game);

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {

        failAndWait();

        return txt_readPossiblePieces(game);
    }

    long pieceCount = strtol(BUFFER, NULL, 10);

    Piece *current = NULL, *prev = NULL;

    for (int i = 0; i < pieceCount; i++) {

        current = readPiece(prev);

        ll_addLast(current, pieces->piecesList);

        prev = current;
    }

    semPost();

    return pieces;
}

static Piece *readPiece(Piece *prev) {

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        perror("FAILED TO READ PIECE.");

        exit(1);
    }

    if (strcmp(BUFFER, "PIECE") == 0) {

        int pieceSize;

        char *name;

        fgets(BUFFER, BUFFER_SIZE, fp);

        char *splitter;

        splitter = strtok(BUFFER, "-");

        name = strdup(splitter);

        splitter = strtok(NULL, "-");

        pieceSize = (int) strtol(splitter, NULL, 10);

        Piece *piece = initPiece(pieceSize, name, readMatrix());

        free(name);

        return piece;

    } else if (strcmp(BUFFER, "REPEAT") == 0) {
        return prev;
    }

    return NULL;
}

static BitMatrix *readMatrix() {

    BitMatrix *matrix = createBitMatrix(MATRIX_ROWS, MATRIX_COLS, MATRIX_WORD_SIZE);

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        perror("FAILED TO READ MATRIX.");

        exit(1);
    }

    char *current = strtok(BUFFER, " ");

    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {

            unsigned int value = (unsigned int) strtol(current, NULL, 10);

            m_setBit(matrix, row, col, value);

            current = strtok(NULL, " ");
        }
    }

    return matrix;
}

void txt_waitForOtherPlayerToChoosePieces() {

    if (!semTryWait()) {
        //If we are the first ones in, (Nothing on the file) write to the file, so that the other one will see and unlock us
        //This can block if process 2 enters between process 1 trying to lock and writing to the file, causing writing to be done twice
        //Not much I can do about that without having to use another named semaphore, so I just decided to leave it be as the odds are pretty low
        if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
            fprintf(fp, "%d\n", 1);

            txt_flush();

            semWait();
        } else {
            semPost();
        }
    }

}

void txt_sendAttemptedPlay(Position *pos, int playerID, int gameID) {
    txt_semWait();

    fprintf(fp, "%d %d %d %d\n", pos->x, pos->y, playerID, gameID);

    txt_flush();

    semPost();
}

Played txt_receiveAttemptedPlay(int game) {

    txt_semWait();

    Played p;

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {

        failAndWait();

        return txt_receiveAttemptedPlay(game);
    }

    int x, y, playerID, gameID;

    char *pnt = strtok(BUFFER, " ");

    x = (int) strtol(pnt, NULL, 10);

    pnt = strtok(NULL, " ");

    y = (int) strtol(pnt, NULL, 10);

    pnt = strtok(NULL, " ");

    playerID = (int) strtol(pnt, NULL, 10);

    pnt = strtok(NULL, " ");

    gameID = (int) strtol(pnt, NULL, 10);

    p.pos = initPos(x, y);

    p.playerID = playerID;

    p.gameID = gameID;

    if (gameID != game) {
        fprintf(stderr, "FAILED TO RECEIVE, GAMEID DOES NOT MATCH %d %d\n", gameID, game);

        exit(1);
    }
    //We don't want to release the semaphore until the response is given, as it could cause some major issues with deadlocks
    return p;
}

void txt_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {

    fprintf(fp, "%d %d %d\n", playerID, hit, gameID);

    txt_flush();

    semPost();
}

HitResult txt_receivedAttemptedPlayResult(int gameID) {

    txt_semWait();

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        perror("Failed to receive attempted play result.\n");

        exit(1);
    }

    char *ptr = strtok(BUFFER, " ");

    int playerID = (int) strtol(ptr, NULL, 10);

    ptr = strtok(NULL, " ");

    int hit = (int) strtol(ptr, NULL, 10);

    ptr = strtok(NULL, " ");

    int game = (int) strtol(ptr, NULL, 10);

    if (game != gameID) {
        fprintf(stderr, "FAILED TO RECEIVE, GAMEID DOES NOT MATCH %d %d\n", gameID, game);

        exit(1);
    }

    HitResult result;

    result.playerID = playerID;
    result.type = hit;

    semPost();

    return result;
}

void txt_destroy() {
    semDestroy();
}