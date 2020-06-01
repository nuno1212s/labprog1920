#include "pipes.h"
#include "sem.h"

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

static void startWriterWithoutMark() {
    currentPosition = 0;

    attemptSemLock();
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

static void writeToBuffer(void *toWrite, size_t size) {

    memcpy(mem_addr + currentPosition, toWrite, size);

    currentPosition += size;

}

static void readFromBuffer(void *toRead, size_t size) {

    memcpy(toRead, mem_addr + currentPosition, size);

    currentPosition += size;
}

int p_readGameSize() {

    startReader();

    int read;

    readFromBuffer(&read, sizeof(int));

    endReader();

    return read;
}

void p_writeGameSize(int gameSize) {

    startWriter();

    writeToBuffer(&gameSize, sizeof(int));

    endWriter();
}

void p_readPlayerInformation(int id, Player *p) {

    startReader();

    int playerID;

    readFromBuffer(&playerID, sizeof(int));

    if (playerID != id) {
        perror("FAILED TO READ PLAYER. (SAME ID)");

        endReader();

        return;
    }

    char *name = malloc(sizeof(char) * MAX_PLAYER_NAME_SIZE);

    readFromBuffer(name, sizeof(char) * MAX_PLAYER_NAME_SIZE);

    p->name = name;

    endReader();
}

void p_sendPlayerInformation(int id, Player *p) {

    startWriter();

    writeToBuffer(&id, sizeof(int));

    writeToBuffer(p->name, sizeof(char) * MAX_PLAYER_NAME_SIZE);

    endWriter();
}

static void writeMatrix(BitMatrix *matrix);

static void p_repeat() {

    char R = 'R';

    writeToBuffer(&R, sizeof(char));

}

static void p_writePiece(Piece *p) {

    char P = 'P';

    writeToBuffer(&P, sizeof(char));

    int nameLength = strlen(p->name);

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

    BitMatrix *matrix = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, MATRIX_WORD_SIZE);

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

void p_sendPossiblePieces(PossiblePieces *pieces) {

    startWriter();

    int size = getPossiblePiecesCount(pieces);

    writeToBuffer(&size, sizeof(int));

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

    readFromBuffer(&pieceType, sizeof(char));

    if (pieceType == 'P') {

        int nameLength;

        readFromBuffer(&nameLength, sizeof(int));

        char *name = malloc(sizeof(char) * nameLength);

        readFromBuffer(name, sizeof(char) * nameLength);

        int pieceSize;

        readFromBuffer(&pieceSize, sizeof(int));

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

    readFromBuffer(&pieceCount, sizeof(int));

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

static int checkCurrentWriter() {

    startWriterWithoutMark();

    int current;

    readFromBuffer(&current, sizeof(int));

    endWriter();

    return current;
}

void p_waitForOtherPlayerToChoosePieces() {

    int result = checkCurrentWriter();

    startWriter();

    endWriter();

    int other = isHost ? 0 : 1;

    while (result != other || result == isHost) {

        sleep(1);

        result = checkCurrentWriter();

        if (result == other) {
            //Seems kind of weird, but this prevents the result from reading -1 after the other user
            //has deleted the file (Notification that he has read and also is ready)
            break;
        }

        printf("Reading player ready... %d (we are %d)\n", result, isHost);
    }

}

void p_sendGameInfo(Game *game) {

    startWriter();

    writeToBuffer(&game->gameID, sizeof(int));

    writeToBuffer(&game->currentPlayerIndex, sizeof(int));

    endWriter();
}

void p_readGameInfo(Game *game) {

    startReader();

    readFromBuffer(&game->gameID, sizeof(int));

    readFromBuffer(&game->currentPlayerIndex, sizeof(int));

    endReader();
}

static Position* p_readPos() {

    int x, y;

    readFromBuffer(&x, sizeof(int));

    readFromBuffer(&y, sizeof(int));

    return initPos(x, y);

}

static void p_sendPos(Position *pos) {

    writeToBuffer(&p_getBaseX(pos), sizeof(int));

    writeToBuffer(&p_getBaseY(pos), sizeof(int));
}

void p_sendAttemptedPlay(Position *pos, int playerID, int gameID) {

    p_sendPos(pos);

    writeToBuffer(&playerID, sizeof(int));

    writeToBuffer(&gameID, sizeof(int));
}

Played p_receiveAttemptedPlay(int gameID) {

    Position *pos = p_readPos();

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

    return p;
}

void p_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {

    writeToBuffer(&playerID, sizeof(int));

    writeToBuffer(&hit, sizeof(int));

    writeToBuffer(&gameID, sizeof(int));

}

HitResult p_receivedAttemptedPlayResult(int gameID) {

    HitResult result;

    readFromBuffer(&result.playerID, sizeof(int));

    readFromBuffer(&result.type, sizeof(int));

    int otherGameID;

    readFromBuffer(&otherGameID, sizeof(int));

    if (otherGameID != gameID) {
        perror("FAILED TO READ ATTEMPTED PLAY. GAMEID DOES NOT MATCH.\n");

        exit(1);
    }

    return result;
}

void p_destroy() {
    if (isHost) {
        closeHost();
    } else {
        closeSlave();
    }
}