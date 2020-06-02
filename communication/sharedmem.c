#include "sharedmem.h"
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

#define SKIP_CHAR 0b11000011

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

    if (mem_addr[currentPosition] == 0) {
        //fprintf(stderr, "Current read pos: %d\n", currentPosition);
        perror("Nothing to read atm....\n");

        return 0;
    }

    if (mem_addr[currentPosition] == (isHost + 1)) {
        perror("The writer is the same as the reader\n");

        return 0;
    }

    currentPosition += sizeof(char);

    return 1;
}

static void markWriter() {
    mem_addr[currentPosition] = (char) (isHost + 1);

    //printf("Marked writer as %d in pos %d\n", isHost + 1, currentPosition);

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

void p_startWriter() {
    attemptSemLock();

    markWriter();
}

static void startWriterWithoutMark() {
    attemptSemLock();
}

void p_startReader() {
    attemptSemLock();

    if (!checkWriter()) {

        finishSemLock();

        sleep(1);

        p_startReader();
    }
}

void p_endReader() {
    finishSemLock();
}

void p_endWriter() {
    finishSemLock();
}


static void prepareForWrite(size_t size) {

    if (currentPosition + size >= MEM_SIZE) {
        mem_addr[currentPosition] = SKIP_CHAR;

        currentPosition = 0;
    }

//    printf("CURRENT POS: %d\n", currentPosition);

}


static void prepareForRead() {

    if (((unsigned char) mem_addr[currentPosition]) == SKIP_CHAR) {
        currentPosition = 0;

        printf("CURRENT POS: %d\n", currentPosition);
    }

}

void p_writeToBuffer(void *toWrite, size_t size) {

    prepareForWrite(size);

    memcpy(mem_addr + currentPosition, toWrite, size);

    currentPosition += size;

    //Set the next byte to 0, to mark that the message ends there
    for (int i = 0; i < sizeof(long) && currentPosition + i < MEM_SIZE; i++) {
        mem_addr[currentPosition + i] = 0;
    }
}

void p_readFromBuffer(void *toRead, size_t size) {

    prepareForRead();

    memcpy(toRead, mem_addr + currentPosition, size);

    currentPosition += size;
}

static char checkCurrentWriter() {

    startWriterWithoutMark();

    char current;

    p_readFromBuffer(&current, sizeof(char));

    p_endWriter();

    return current;
}

void p_waitForOtherPlayerToChoosePieces() {

    char result = checkCurrentWriter();

    p_startWriter();

    //Write the current player tag and regress to keep reading that same position for changes
    currentPosition -= sizeof(char);
    p_endWriter();

    int other = (isHost == 1) ? 1 : 2;

    while (result != other) {

        sleep(1);

        result = checkCurrentWriter();
        currentPosition -= sizeof(char);

        printf("Reading player ready... %d (we are %d)\n", result, isHost + 1);
    }

    p_startWriter();

    p_endWriter();

    printf("READ PLAYER READY %d\n", result);

}

void p_destroy() {
    if (isHost) {
        closeHost();
    } else {
        closeSlave();
    }
}