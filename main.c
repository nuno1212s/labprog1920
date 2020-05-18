#include <stdio.h>

#include "graphics/graphics.h"
#include "communication/sem.h"
#include <unistd.h>
#include <stdlib.h>

#include "communication/communications.h"

#define HOST 0
#define SLAVE 1

void startGame(int arg) {

    printf("INIT COMS\n");

    int host = arg == HOST ? 1 : 0;

    initComms(TEXT, host);

    initSem(host);

    if (arg == 0) {
        printf("SENDING...\n");

        Position pos;

        pos.x = 10;
        pos.y = 20;

        printf("WAITING\n");
        semWait();

        printf("WAITED\n");

        sendAttemptedPlay(&pos, 1, 1);

        printf("SENT\n");

        sleep(5);

        semPost();

        int x;

        scanf("%d", &x);

    } else {

        int x;

        printf("RECEIVING IN 2...\n");

        sleep(2);

        semWait();

        printf("RECEIVING...\n");

        Played played = receiveAttemptedPlay(1);

        printf("RECEIVED\n");

        semPost();

        scanf("%d", &x);

        printf("POS: %d %d PLAYERID: %d\n", p_getBaseX(played.pos), p_getBaseY(played.pos), played.playerID);
    }

    semDestroy();
}

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");

    for (int i = 0; i < argc; i++) {
        printf("Arg %d: %s \n", i, argv[i]);
    }

    int split = 0;

    if (argc >= 2) {
        split = (int) strtol(argv[1], NULL, 10);
    } else {
        printf("Insert %d for host\n", HOST);

        scanf("%d", &split);
    }

    if (split == HOST) {
        pid_t pid = fork();

        char cwd[1024];

        getcwd(cwd, 1024);

        if (pid == -1) {
            fprintf(stderr, "Failed to fork process");
        } else if (pid == 0) {

            execlp("gnome-terminal", "gnome-terminal", cwd, "-e", "./main 1", NULL);

        } else {

            startGame(split);

        }
    } else {
        startGame(split);
    }

    return 0;
}
