#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "buffered_communication.h"

#ifdef SHARED_MEM

#include "sharedmem.h"

#elif defined(NETWORK)

#include "sockets.h"

#endif

//Definitions for functions to control the buffer writes / reads
static void startWriter();

static void writeToBuffer(void *toWrite, size_t size);

static void endWriter();

static void startReader();

static void readFromBuffer(void *toReadTo, size_t size);

static void endReader();

#ifdef SHARED_MEM

void bf_init(int host) {
    p_init(host);
}

void bf_block() {
    p_block();
}

void startWriter() {
    p_startWriter();
}

void writeToBuffer(void *toWrite, size_t size) {
    p_writeToBuffer(toWrite, size);
}

void endWriter() {
    p_endWriter();
}

void startReader() {
    p_startReader();
}

void readFromBuffer(void *toRead, size_t size) {
    p_readFromBuffer(toRead, size);
}

void endReader() {
    p_endReader();
}

void bf_waitForOtherPlayerToChoosePieces() {
    p_waitForOtherPlayerToChoosePieces();
}

void bf_destroy() {
    p_destroy();
}

#elif defined(NETWORK)

void bf_init(int host) {
    s_init(host);
}

void bf_block() {
    //There is no need for this on networking
}

void startWriter() {
    s_startWriter();
}

void writeToBuffer(void *toWrite, size_t size) {
    s_writeToBuffer(toWrite, size);
}

void endWriter() {
    s_endWriter();
}

void startReader() {
    s_startReader();
}

void readFromBuffer(void *toRead, size_t size) {
    s_readFromBuffer(toRead, size);
}

void endReader() {
    s_endReader();
}

void bf_waitForOtherPlayerToChoosePieces() {
    s_waitForOtherPlayerToChoosePieces();
}

void bf_destroy() {
    s_destroy();
}

#endif

int bf_readGameSize() {
    startReader();

    int read;

    readFromBuffer(&read, sizeof(int));

    endReader();

    return read;
}

void bf_writeGameSize(int gameSize) {
    startWriter();

    writeToBuffer(&gameSize, sizeof(int));

    endWriter();
}

void bf_readPlayerInformation(int id, Player *pl) {
    startReader();

    int playerID;

    readFromBuffer(&playerID, sizeof(int));

    if (playerID != id) {
        fprintf(stderr, "FAILED TO READ PLAYER. (DIFFERENT ID %d, %d)", playerID, id);

        endReader();

        exit(1);
    }

    int playerNameLength;

    readFromBuffer(&playerNameLength, sizeof(int));

    char *name = calloc(playerNameLength, sizeof(char));

    readFromBuffer(name, sizeof(char) * playerNameLength);

    pl_setname(pl, name);

    endReader();
}

void bf_sendPlayerInformation(int id, Player *p) {

    startWriter();

    writeToBuffer(&id, sizeof(int));

    int playerNameLen = strlen(pl_name(p)) + 1;

    writeToBuffer(&playerNameLen, sizeof(int));

    writeToBuffer(pl_name(p), sizeof(char) * playerNameLen);

    endWriter();
}

static void writeMatrix(BitMatrix *matrix);

static void bf_repeat() {

    char R = 'R';

    writeToBuffer(&R, sizeof(char));

}

static void bf_writePiece(Piece *p) {

    char P = 'P';

    writeToBuffer(&P, sizeof(char));

    int nameLength = strlen(p->name) + 1;

    writeToBuffer(&nameLength, sizeof(int));

    writeToBuffer(p->name, sizeof(char) * nameLength);

    int pieceSize = p->size;

    writeToBuffer(&pieceSize, sizeof(int));

    int chars = PIECE_MATRIX_COLS * PIECE_MATRIX_ROWS;

    writeToBuffer(&chars, sizeof(int));

    writeMatrix(p->matrix);
}

void writeMatrix(BitMatrix *matrix) {

    for (int row = 0; row < matrix_rows(matrix); row++) {

        for (int col = 0; col < matrix_cols(matrix); col++) {

            unsigned char value = (unsigned char) m_getBit(matrix, row, col);

            writeToBuffer(&value, sizeof(unsigned char));
        }
    }

}

BitMatrix *readMatrix() {

    BitMatrix *matrix = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, PIECE_MATRIX_WORD_SIZE);

    int chars;

    readFromBuffer(&chars, sizeof(int));

    for (int row = 0; row < matrix_rows(matrix); row++) {
        for (int col = 0; col < matrix_cols(matrix); col++) {

            char value;

            readFromBuffer(&value, sizeof(char));

            m_setBit(matrix, row, col, value);
        }
    }

    return matrix;
}

void bf_sendPossiblePieces(PossiblePieces *pieces) {
    startWriter();

    int size = getPossiblePiecesCount(pieces);

    writeToBuffer(&size, sizeof(int));

    struct Node_s *node = pieces->piecesList->first;

    Piece *prevPiece = NULL,
            *currentPiece = NULL;

    while (node != NULL) {
        currentPiece = node->data;

        if (prevPiece == currentPiece) {
            bf_repeat();
        } else {
            bf_writePiece(currentPiece);
        }

        prevPiece = currentPiece;
        node = node->next;
    }

    endWriter();
}

static Piece *bf_readPiece(Piece *prev) {

    char pieceType;

    readFromBuffer(&pieceType, sizeof(char));

    if (pieceType == 'P') {

        int nameLength;

        readFromBuffer(&nameLength, sizeof(int));

        char name[nameLength];

        readFromBuffer(name, sizeof(char) * nameLength);

        int pieceSize;

        readFromBuffer(&pieceSize, sizeof(int));

        Piece *piece = initPiece(pieceSize, name, readMatrix());

        return piece;

    } else if (pieceType == 'R') {
        return prev;
    } else {
        printf("FAILED TO READ %c .\n", pieceType);

        exit(1);
    }

}

PossiblePieces *bf_receivePossiblePieces(Game *game) {

    startReader();

    int pieceCount;

    readFromBuffer(&pieceCount, sizeof(int));

    PossiblePieces *pieces = initPossiblePieces(game);

    Piece *current = NULL, *prev = NULL;

    for (int i = 0; i < pieceCount; i++) {

        current = bf_readPiece(prev);

        ll_addLast(current, pieces->piecesList);

        prev = current;
    }

    endReader();

    return pieces;
}

void bf_sendGameInfo(Game *game) {
    startWriter();

    writeToBuffer(&game->gameID, sizeof(int));

    writeToBuffer(&game->currentPlayerIndex, sizeof(int));

    endWriter();
}

void bf_readGameInfo(Game *game) {
    startReader();

    readFromBuffer(&game->gameID, sizeof(int));

    readFromBuffer(&game->currentPlayerIndex, sizeof(int));

    endReader();
}

static Position *bf_readPos() {

    int x, y;

    readFromBuffer(&x, sizeof(int));

    readFromBuffer(&y, sizeof(int));

    return initPos(x, y);

}

static void bf_sendPos(Position *pos) {

    writeToBuffer(&p_getBaseX(pos), sizeof(int));

    writeToBuffer(&p_getBaseY(pos), sizeof(int));
}

void bf_sendAttemptedPlay(Position *pos, int playerID, int gameID) {

    startWriter();

    bf_sendPos(pos);

    writeToBuffer(&playerID, sizeof(int));

    writeToBuffer(&gameID, sizeof(int));

    endWriter();
}

Played bf_receiveAttemptedPlay(int gameID) {

    startReader();

    Position *pos = bf_readPos();

    int playerID, otherGameID;

    readFromBuffer(&playerID, sizeof(int));

    readFromBuffer(&otherGameID, sizeof(int));

    if (gameID != otherGameID) {
        perror("FAILED TO READ, GAMEID IS NOT THE SAME\n");

        exit(1);
    }

    Played p;

    p.pos = pos;
    p.playerID = playerID;
    p.gameID = otherGameID;

    endReader();

    return p;
}


void bf_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {

    startWriter();

    writeToBuffer(&playerID, sizeof(int));

    writeToBuffer(&hit, sizeof(int));

    writeToBuffer(&gameID, sizeof(int));

    endWriter();

}

HitResult bf_receivedAttemptedPlayResult(int gameID) {

    startReader();

    HitResult result;

    readFromBuffer(&result.playerID, sizeof(int));

    readFromBuffer(&result.type, sizeof(int));

    int otherGameID;

    readFromBuffer(&otherGameID, sizeof(int));

    if (otherGameID != gameID) {
        perror("FAILED TO READ ATTEMPTED PLAY. GAMEID DOES NOT MATCH.\n");

        exit(1);
    }

    endReader();

    return result;
}