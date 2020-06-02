#include <stdio.h>

#include "graphics/graphics.h"
#include <unistd.h>
#include <stdlib.h>

#define HOST 1
#define SLAVE 0

static void initShells(int argc, char *argv[]);

#define TWO_SHELL

#ifdef ONE_SHELL

void initShells(int argc, char *argv[]) {

    startGame(1);

}

#elif defined(TWO_SHELL)

void initShells(int argc, char *argv[]) {

    int split = 0;

    if (argc >= 2) {
        split = (int) strtol(argv[1], NULL, 10);
    } else {
        printf("Insert %d for host\n", HOST);

        scanf("%d", &split);
    }

    if (split == HOST) {
        startGame(split);

        return;
        sleep(1);
        pid_t pid = fork();

        char cwd[1024];

        getcwd(cwd, 1024);

        if (pid == -1) {
            fprintf(stderr, "Failed to fork process");
        } else if (pid == 0) {

            execlp("gnome-terminal", "gnome-terminal", cwd, "-e", "./LabProg 0", NULL);

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
