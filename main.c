#include <stdio.h>

#define HOST 1
#define SLAVE 0

static void initShells(int argc, char *argv[]);

#ifdef ONE_SHELL

void initShells(int argc, char *argv[]) {

    startGame(1);

}

#elif defined(TWO_SHELL)

#include "graphics/graphics.h"
#include <unistd.h>
#include <stdlib.h>

void initShells(int argc, char *argv[]) {

    int split = 0;

    if (argc >= 2) {
        split = (int) strtol(argv[1], NULL, 10);
    } else {
        //If no args provided on startup, assumed to be the host.
        split = HOST;
    }

    if (split == HOST) {
#ifdef NETWORK
        printf("STARTING THE HOST PROGRAM.\n");

        printf("1) Start the client program on this machine. (The IP Address is 127.0.0.1 in this case)\n");

        printf("2) Start the client program on another machine. (Requires user action, use ./battleship 0 to start the program as client)\n");

        int choice;

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                break;
            case 2:
                startGame(split);
                return;
            default:
                printf("INVALID OPTION, Starting client on this machine...\n");
                break;
        }
#endif
        pid_t pid = fork();

        char cwd[1024];

        getcwd(cwd, 1024);

        if (pid == -1) {
            fprintf(stderr, "Failed to fork process");
        } else if (pid == 0) {

            char cmd[128];

            snprintf(cmd, 128, "%s 0", argv[0]);

            execlp("gnome-terminal", "gnome-terminal", cwd, "-e", cmd, NULL);

        } else {

            startGame(split);

        }
    } else {
        startGame(split);
    }
}

#endif

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");

    initShells(argc, argv);

    return 0;
}
