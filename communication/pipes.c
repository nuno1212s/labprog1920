#include "pipes.h"
#include "sem.h"
#include "../structures/pieces.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BACKING_FILE "/shmBattleShip"

#define MEM_SIZE 1024

static int isHost;

static int currentPosition = 0;

static int fileP;

static caddr_t mem_addr;

static void initHost();

static void initSlave();

void p_init(int host) {

    isHost = host;

    initSem(host);

    if (host) {
        initHost();
    } else {
        initSlave();
    }

}

static void initHost() {
    fileP = shm_open(BACKING_FILE, O_RDWR | O_CREAT, ACCESSPERMS);

    if (fileP < 0) {
        perror("Can't open shared memory.");

        return;
    }

    ftruncate(fileP, MEM_SIZE);

    mem_addr = mmap(NULL, MEM_SIZE,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    fileP,
                    0);

    if ((caddr_t) -1 == mem_addr) {
        perror("Can't get segment.\n");

        return;
    }
}

static void initSlave() {

    fileP = shm_open(BACKING_FILE, O_RDWR, ACCESSPERMS);

    if (fileP < 0) {
        perror("Can't get segment.\n");

        exit(1);
    }

    mem_addr = mmap(NULL,
                    MEM_SIZE,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    fileP,
                    0);

    if ((caddr_t) -1 == mem_addr) {
        perror("Can't get access to segment..\n");

        exit(1);
    }

}

static int checkWriter() {

    if (mem_addr[currentPosition] == isHost) {
        perror("The writer is the same as the reader");

        return 0;
    }

    currentPosition += sizeof(char);

    return 1;
}

static void markWriter() {
    mem_addr[currentPosition] = (char) isHost;

    currentPosition += sizeof(char);
}

static void closeSlave() {
    munmap(mem_addr, MEM_SIZE);
    close(fileP);

    unlink(BACKING_FILE);
}

static void closeHost() {
    munmap(mem_addr, MEM_SIZE);
    close(fileP);

    shm_unlink(BACKING_FILE);
}

int isBlocking = 0;

void p_block() {

    if (isBlocking) return;

    isBlocking = 1;

    semWait();

}

static void attemptSemLock() {
    if (isBlocking) {
        isBlocking = 0;
        return;
    }

    semWait();
}

static void finishSemLock() {
    semPost();
}

static void startWriter() {
    currentPosition = 0;

    attemptSemLock();

    markWriter();
}

static void startReader() {
    currentPosition = 0;

    attemptSemLock();

    if (!checkWriter()) {

        finishSemLock();

        sleep(1);

        startReader();
    }
}

static void endReader() {
    finishSemLock();

    for (int i = 0; i < MEM_SIZE; i++) {
        //Clear the memory
        mem_addr[i] = 0;
    }
}

static void endWriter() {
    finishSemLock();
}

int p_readGameSize() {

    startReader();

    int read;

    memcpy(&read, mem_addr + currentPosition, sizeof(int));

    currentPosition += sizeof(int);

    endReader();

    return read;
}

void p_writeGameSize(int gameSize) {

    startWriter();

    memcpy(mem_addr + currentPosition, &gameSize, sizeof(int));

    currentPosition += sizeof(int);

    endWriter();
}

void p_readPlayerInformation(int id, Player *p) {

    startReader();

    int playerID;

    memcpy(&playerID, mem_addr + currentPosition, sizeof(int));

    currentPosition += sizeof(int);

    if (playerID != id) {
        perror("FAILED TO READ PLAYER. (SAME ID)");

        endReader();

        return;
    }

    char *name = malloc(sizeof(char) * MAX_PLAYER_NAME_SIZE);

    memcpy(name, mem_addr + currentPosition, sizeof(char) * MAX_PLAYER_NAME_SIZE);

    currentPosition += sizeof(char) * MAX_PLAYER_NAME_SIZE;

    p->name = name;

    endReader();
}

void p_sendPlayerInformation(int id, Player *p) {

    startWriter();

    memcpy(mem_addr + currentPosition, &id, sizeof(int));

    currentPosition += sizeof(int);

    memcpy(mem_addr + currentPosition, p->name, sizeof(char) * MAX_PLAYER_NAME_SIZE);

    currentPosition += sizeof(char) * MAX_PLAYER_NAME_SIZE;

    endWriter();
}

static void writeMatrix(BitMatrix *matrix);

static void p_repeat() {

    mem_addr[currentPosition] = 'R';

    currentPosition += sizeof(char);

}

static void p_writePiece(Piece *p) {

    mem_addr[currentPosition] = 'P';

    currentPosition += sizeof(char);

    int nameLength = strlen(p->name);

    memcpy(mem_addr + currentPosition, &nameLength, sizeof(int));

    currentPosition += sizeof(int);

    memcpy(mem_addr + currentPosition, p->name, sizeof(char) * nameLength);

    currentPosition += sizeof(char) * nameLength;

    int pieceSize = p->size;

    memcpy(mem_addr + currentPosition, &pieceSize, sizeof(int));

    currentPosition += sizeof(int);

    int chars = PIECE_MATRIX_COLS * PIECE_MATRIX_ROWS;

    memcpy(mem_addr + currentPosition, &chars, sizeof(int));

    currentPosition += sizeof(int);

    writeMatrix(p->matrix);
}

void writeMatrix(BitMatrix *matrix) {

    for (int row = 0; row < matrix_rows(matrix); row++) {

        for (int col = 0; col < matrix_cols(matrix); col++) {

            unsigned char value = (unsigned char) m_getBit(matrix, row, col);

            memcpy(mem_addr + currentPosition, &value, sizeof(unsigned char));

            currentPosition += sizeof(unsigned char);
        }
    }

}

BitMatrix *readMatrix() {

    BitMatrix *matrix = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, MATRIX_WORD_SIZE);

    int chars;

    memcpy(&chars, mem_addr + currentPosition, sizeof(int));

    currentPosition += sizeof(int);

    for (int row = 0; row < matrix_rows(matrix); row++) {
        for (int col = 0; col < matrix_cols(matrix); col++) {

            char value;

            memcpy(&value, mem_addr + currentPosition, sizeof(char));

            currentPosition += sizeof(char);

            m_setBit(matrix, row, col, value);
        }
    }

    return matrix;
}

void p_sendPossiblePieces(PossiblePieces *pieces) {

    startWriter();

    int size = getPossiblePiecesCount(pieces);

    memcpy(mem_addr + currentPosition, &size, sizeof(int));

    currentPosition += sizeof(int);

    struct Node_s *node = pieces->piecesList->first;

    Piece *prevPiece = NULL,
            *currentPiece = NULL;

    while (node != NULL) {
        currentPiece = node->data;

        if (prevPiece == currentPiece) {
            p_repeat();
        } else {
            p_writePiece(currentPiece);
        }

        prevPiece = currentPiece;
        node = node->next;
    }

    endWriter();
}

Piece *p_readPiece(Piece *prev) {

    char pieceType;

    memcpy(&pieceType, mem_addr + currentPosition, sizeof(char));

    currentPosition += sizeof(char);

    if (pieceType == 'P') {

        int nameLength;

        memcpy(&nameLength, mem_addr + currentPosition, sizeof(int));

        currentPosition += sizeof(int);

        char *name = malloc(sizeof(char) * nameLength);

        memcpy(name, mem_addr + currentPosition, sizeof(char) * nameLength);

        currentPosition += sizeof(char) * nameLength;

        int pieceSize;

        memcpy(&pieceSize, mem_addr + currentPosition, sizeof(int));

        currentPosition += sizeof(int);

        Piece *piece = initPiece(pieceSize, name, readMatrix());

        return piece;

    } else if (pieceType == 'R') {
        return prev;
    } else {
        printf("FAILED TO READ %c\n", pieceType);

        exit(1);
    }

}

PossiblePieces *p_receivePossiblePieces(Game *game) {

    startReader();

    int pieceCount;

    memcpy(&pieceCount, mem_addr + currentPosition, sizeof(int));

    currentPosition += sizeof(int);

    PossiblePieces *pieces = initPossiblePieces(game);

    Piece *current = NULL, *prev = NULL;

    for (int i = 0; i < pieceCount; i++) {

        current = p_readPiece(prev);

        ll_addLast(current, pieces->piecesList);

        prev = current;
    }

    endReader();

    return pieces;
}