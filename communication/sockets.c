#include "sockets.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>

#define BUFF_SIZE 1024
#define PORT 8080

#define IP_ADDR "127.0.0.1"

static int isHost;

static int server_fd, openSocket_fd;

static struct sockaddr_in address;

static int currentBufferPos = 0;

char BUFFER[BUFF_SIZE] = {0};

static void initHost() {

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {

        perror("Failed opening socket...\n");

        exit(1);
    }

    int opt = 1;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setting the port of the socket has failed");

        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int addrlen = sizeof(address);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Failed to bind socket");

        exit(1);
    }

    printf("Waiting for client to connect...\n");

    if (listen(server_fd, 1) < 0) {
        perror("Failed to listen");

        exit(1);
    }

    if ((openSocket_fd = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {

        perror("Failed to accept connection");

        exit(1);
    }

    printf("Opened communication on channel %d\n", openSocket_fd);
}

static void initSlave() {

    if ((openSocket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket\n");

        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, IP_ADDR, &address.sin_addr) <= 0) {
        fprintf(stderr, "INVALID IP ADDR %s\n", IP_ADDR);

        exit(1);
    }

    if (connect(openSocket_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("CONNECTION FAILED \n");

        exit(1);
    }

    printf("Opened connection to server on fd %d\n", openSocket_fd);

}

void s_init(int host) {

    isHost = host;

    if (host) {
        initHost();
    } else {
        initSlave();
    }

    //Make the sockets block on read when there is no information to read
    //Instead of just reading 0
    ioctl(openSocket_fd, FIONBIO, 0);
}

void s_startWriter() {
    currentBufferPos = 0;
}

void s_writeToBuffer(void *toWrite, size_t size) {

    memcpy(BUFFER + currentBufferPos, toWrite, size);

    currentBufferPos += size;
}

void s_endWriter() {

    send(openSocket_fd, BUFFER, currentBufferPos, 0);

    printf("Sending %d bytes\n", currentBufferPos);

    currentBufferPos = 0;

}

void s_startReader() {

    currentBufferPos = 0;

    int readBytes = read(openSocket_fd, BUFFER, BUFF_SIZE);

    printf("Reading %d bytes\n", readBytes);

}

void s_readFromBuffer(void *toRead, size_t size) {

    memcpy(toRead, BUFFER + currentBufferPos, size);

    currentBufferPos += size;

}

void s_endReader() {}

void s_waitForOtherPlayerToChoosePieces() {

    s_startWriter();

    s_writeToBuffer(&isHost, sizeof(int));

    s_endWriter();

    //Block until we receive the communication from the other client
    s_startReader();

    s_endReader();

}

void s_destroy() {

    if (isHost) {
        close(openSocket_fd);
        close(server_fd);
    } else {
        close(openSocket_fd);
    }

}