#include <stdio.h>

#include "graphics/graphics.h"
#include "communication/sem.h"
#include <unistd.h>
#include <stdlib.h>

#include "communication/communications.h"

#define HOST 1
#define SLAVE 0

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
        startGame(split);
        /*
        pid_t pid = fork();

        char cwd[1024];

        getcwd(cwd, 1024);

        if (pid == -1) {
            fprintf(stderr, "Failed to fork process");
        } else if (pid == 0) {

            execlp("gnome-terminal", "gnome-terminal", cwd, "-e", "./LabProg 0", NULL);

        } else {

            startGame(split);

        }*/
    } else {
        startGame(split);
    }

    return 0;
}
