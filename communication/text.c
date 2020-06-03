#include "text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define TEXT_FILE "gameinfo.txt"

#define BUFFER_SIZE 1024

static char BUFFER[BUFFER_SIZE];

static int host;

static FILE *fp;

static int isBlocking = 0, isReadLock = 0;

static struct flock lock;

/**
 * Preemptive method declarations
 */
static void txt_repeatPiece();

static void txt_writePiece(Piece *);

static Piece *readPiece(Piece *);

static BitMatrix *readMatrix();

static void closeFP();

static void markWriter() {

    fprintf(fp, "%d\n", host);

}

static int checkWriter() {

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        printf("Failed to read the writer\n");

        return 0;
    }

    int writer = (int) strtol(BUFFER, NULL, 10);

    if (writer == host) {
        printf("Writer is the same as the reader.");

        return 0;
    }

    return 1;
}

/**
 * Init the file pointer and get the file write lock
 *
 * @param ignore Whether the file should ignore if there are already stuff there
 */
static void initWriteFP(int ignore) {

    if (isBlocking == 1) {
        isBlocking = 0;
        return;
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; //Until EOF
    lock.l_pid = getpid();

    isReadLock = 0;

    if ((fp = fopen(TEXT_FILE, "r+")) <= 0) {

        perror("Failed to open file...\n");

        sleep(5);

        exit(1);
    }

    printf("Attempting to obtain lock for write file...\n");

    printf("FPN: %d\n", fileno(fp));

    if (fcntl(fileno(fp), F_SETLKW, &lock) < 0) {
        perror("FAILED TO LOCK FILE.\n");

        exit(1);
    }

    if (!ignore) {
        struct stat fileStat;

        fstat(fileno(fp), &fileStat);

        if (fileStat.st_size != 0) {

            perror("FILE IS NOT EMPTY, WAITING FOR OTHER READ\n");

            closeFP();

            sleep(1);

            initWriteFP(0);
        } else {
            printf("File is good for use\n");
        }
    }

}

/**
 * Initialize the reading file pointer.
 */
static void initReadFP() {

    if (isBlocking == 1) {
        isBlocking = 0;
        return;
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; //Until EOF
    lock.l_pid = getpid();

    isReadLock = 1;

    if ((fp = fopen(TEXT_FILE, "r+")) <= 0) {
        perror("Failed to open file...\n");

        sleep(5);

        exit(1);
    }

    printf("Attempting to obtain lock for read file...\n");
    printf("FPN: %d\n", fileno(fp));

    if (fcntl(fileno(fp), F_SETLKW, &lock) < 0) {
        perror("FAILED TO LOCK FILE.\n");

        exit(1);
    }

}

static void startWriter() {
    if (isBlocking) {
        isBlocking = 0;

        return;
    }

    initWriteFP(0);

    markWriter();
}

static void startRead() {

    initReadFP();

    if (!checkWriter()) {
        //If the writer is this instance of the game, then we don't want to read it as it was meant for the other instance.
        isReadLock = 0;
        closeFP();

        sleep(1);

        startRead();
    }

}

static void closeFP() {

    lock.l_type = F_UNLCK;

    if (fcntl(fileno(fp), F_SETLK, &lock) < 0) {
        perror("Could not unlock file *\n");

        exit(1);
    }

    if (isReadLock) {
        //Reset the file after reading
        ftruncate(fileno(fp), 0);
    }

    fclose(fp);

}

void failAndWait() {

    //Release the file so that others can do their work
    closeFP();

    perror("FAILED TO READ FROM FILE... WAITING 3 SECONDS FOR OTHER TERMINAL...");

    sleep(3);

}

void txt_flush() {
    fflush(fp);
}

void txt_init(int isHost) {
    host = isHost;

    //Create the file
    FILE *file = fopen(TEXT_FILE, "w");

    fclose(file);
}

void txt_block() {

    if (isBlocking) return;

    printf("TXT_BLOCK CALLED \n");

    startWriter();

    printf("TXTBLOCK INITIALIZED\n");

    isBlocking = 1;
}

static char *readFromFP() {

    char *result;

    if ((result = fgets(BUFFER, BUFFER_SIZE, fp)) == 0) return NULL;

    for (int i = 0;; i++) {

        if (result[i] == '\n') {
            result[i] = '\0';
            break;
        }

    }

    return result;

}

int txt_readGameSize() {

    startRead();

    if (readFromFP() == 0) {
        failAndWait();

        return txt_readGameSize();
    }

    int gameSize = (int) strtol(BUFFER, NULL, 10);

    closeFP();

    return gameSize;
}

void txt_sendGameSize(int gameSize) {

    startWriter();

    fprintf(fp, "%d\n", gameSize);

    txt_flush();

    closeFP();
}

void txt_writePossiblePieces(PossiblePieces *pieces) {

    startWriter();

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

    closeFP();
}


void txt_writePlayerInfo(int id, char *name) {
    startWriter();

    fprintf(fp, "%d %s\n", id, name);

    txt_flush();

    closeFP();
}

void txt_sendPlayerInformation(int id, Player *player) {
    txt_writePlayerInfo(id, pl_name(player));
}

void txt_readPlayerInformation(int id, Player *player) {

    startRead();

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {

        failAndWait();

        txt_readPlayerInformation(id, player);

        return;
    }

    char *current = strtok(BUFFER, " ");

    int playerID = (int) strtol(current, NULL, 10);

    char *name = strtok(NULL, " ");

    if (playerID != id) {
        printf("FAILED TO READ PLAYER. (Same player)");

        closeFP();

        txt_writePlayerInfo(id, name);

        return;
    }

    pl_setname(player, strdup(name));

    closeFP();
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

    startRead();

    PossiblePieces *pieces = initPossiblePieces(game);

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {

        failAndWait();

        return txt_readPossiblePieces(game);
    }

    long pieceCount = strtol(BUFFER, NULL, 10);

    Piece *current = NULL, *prev = NULL;

    printf("There are %ld pieces\n", pieceCount);

    for (int i = 0; i < pieceCount; i++) {

        printf("Reading piece... %d\n", i);

        current = readPiece(prev);

        ll_addLast(current, pieces->piecesList);

        prev = current;
    }

    closeFP();

    return pieces;
}

static Piece *readPiece(Piece *prev) {

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        perror("FAILED TO READ PIECE.");

        exit(1);
    }

    printf("Buffer: %s\n", BUFFER);

    if (strcmp(BUFFER, "PIECE\n") == 0) {

        int pieceSize;

        char *name;

        fgets(BUFFER, BUFFER_SIZE, fp);

        char *splitter;

        splitter = strtok(BUFFER, "-");

        name = strdup(splitter);

        printf("Reading piece: %s\n", name);

        splitter = strtok(NULL, "-");

        pieceSize = (int) strtol(splitter, NULL, 10);

        printf("Piece size: %d\n", pieceSize);

        Piece *piece = initPiece(pieceSize, name, readMatrix());

        free(name);

        return piece;

    } else if (strcmp(BUFFER, "REPEAT\n") == 0) {
        return prev;
    }

    return NULL;
}

static BitMatrix *readMatrix() {

    BitMatrix *matrix = createBitMatrix(PIECE_MATRIX_ROWS, PIECE_MATRIX_COLS, PIECE_MATRIX_WORD_SIZE);

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        perror("FAILED TO READ MATRIX.");

        exit(1);
    }

    char *current = strtok(BUFFER, " ");

    for (int row = 0; row < PIECE_MATRIX_ROWS; row++) {
        for (int col = 0; col < PIECE_MATRIX_COLS; col++) {

            unsigned int value = (unsigned int) strtol(current, NULL, 10);

            m_setBit(matrix, row, col, value);

            current = strtok(NULL, " ");
        }
    }

    return matrix;
}

void txt_sendGameInfo(Game *game) {
    startWriter();

    fprintf(fp, "%d %d\n", g_gameID(game), g_currentPlayer(game));

    closeFP();
}

void txt_readGameInfo(Game *game) {
    startRead();

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {
        perror("Failed to read game info\n");

        exit(1);
    }

    char *data = strtok(BUFFER, " ");

    int gameID = (int) strtol(data, NULL, 10);

    data = strtok(NULL, " ");

    int currentPlayer = (int) strtol(data, NULL, 10);

    g_currentPlayer(game) = currentPlayer;
    g_gameID(game) = gameID;

    closeFP();
}

static int checkFile() {

    initWriteFP(1);

    if (fgets(BUFFER, BUFFER_SIZE, fp) == 0) {

        closeFP();

        return -1;

    } else {

        int read = (int) strtol(BUFFER, NULL, 10);

        closeFP();

        return read;
    }

}

void txt_waitForOtherPlayerToChoosePieces() {

    int result = checkFile();

    initWriteFP(1);

    ftruncate(fileno(fp), 0);

    fprintf(fp, "%d\n", host);

    closeFP();

    while (result == -1 || result == host) {
        sleep(1);

        result = checkFile();

        if (result == -1) {
            //Seems kind of weird, but this prevents the result from reading -1 after the other user has
            //deleted the file (Notification that he has read and also is ready)
            break;
        }

        printf("Reading player ready... %d (we are %d)\n", result, host);
    }

    initWriteFP(1);

    ftruncate(fileno(fp), 0);

    closeFP();

    printf("Other player ready!\n");
}

void txt_sendAttemptedPlay(Position *pos, int playerID, int gameID) {
    startWriter();

    fprintf(fp, "%d %d %d %d\n", pos->x, pos->y, playerID, gameID);

    txt_flush();

    closeFP();
}

Played txt_receiveAttemptedPlay(int game) {

    startRead();

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

    closeFP();

    //We don't want to release the semaphore until the response is given, as it could cause some major issues with deadlocks
    return p;
}

void txt_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {

    startWriter();

    fprintf(fp, "%d %d %d\n", playerID, hit, gameID);

    txt_flush();

    //Close the lock when we are done with the file
    closeFP();
}

HitResult txt_receivedAttemptedPlayResult(int gameID) {

    startRead();

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

    closeFP();

    return result;
}

void txt_destroy() {

    initWriteFP(1);

    ftruncate(fileno(fp), 0);

    closeFP();
}