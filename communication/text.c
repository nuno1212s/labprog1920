#include "text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../storagestructures/bitmap.h"
#include "../storagestructures/linkedlist.h"

#define TEXT_FILE "gameinfo.txt"

#define BUFFER_SIZE 1024

static FILE *fp;

static char BUFFER[BUFFER_SIZE];

static void txt_repeatPiece();
static void txt_writePiece(Piece*);

static Piece * readPiece(Piece*);
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

void txt_flush() {
    fflush(fp);
}

void txt_init(int host) {
    initFP(host);
}

void txt_writePossiblePieces(PossiblePieces *pieces) {

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
}

void txt_sendPlayerInformation(Player *player) {

    fprintf(fp, "%s\n", player->name);

}

void txt_readPlayerInformation(Player *player) {

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {

        perror("FAILED TO READ PLAYER INFO.");

        exit(1);
    }

    player->name = strdup(BUFFER);
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

PossiblePieces *txt_readPossiblePieces() {

    PossiblePieces *pieces = malloc(sizeof(PossiblePieces));

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        perror("FAILED TO READ POSSIBLE PIECES.");

        exit(1);
    }

    long pieceCount = strtol(BUFFER, NULL, 10);

    Piece *current= NULL, *prev = NULL;

    for (int i = 0; i < pieceCount; i++) {

        current = readPiece(prev);

        ll_addLast(current, pieces->piecesList);

        prev = current;
    }

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

void txt_sendAttemptedPlay(Position *pos, int playerID, int gameID) {
    fprintf(fp, "%d %d %d %d\n", pos->x, pos->y, playerID, gameID);

    txt_flush();
}

Played txt_receiveAttemptedPlay(int game) {

    Played p;

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        perror("FAILED TO READ ATTEMPTED PLAY.");

        exit(1);
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

    return p;
}

void txt_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {

    fprintf(fp, "%d %d %d\n", playerID, hit, gameID);

    txt_flush();
}

HitResult txt_receivedAttemptedPlayResult(int gameID) {

    fgets(BUFFER, BUFFER_SIZE, fp);

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

    return result;
}