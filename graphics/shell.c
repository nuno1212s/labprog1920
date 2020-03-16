//
// Created by nuno on 16/03/20.
//

#include "shell.h"
#include "../structures/game.h"

#include <stdio.h>
#include <stdlib.h>

void sg_displayWhereToPlay(char *);

void sg_showOffline();

void sg_displayMenu() {

    printf("# Hello, Welcome to battle of ships.\n");
    printf("Please insert your player name (MAX 25 chars):\n");

    char name[26] = {'\0'};

    scanf("%25s", name);

    sg_displayWhereToPlay(name);
}

void sg_displayWhereToPlay(char *name) {

    printf("# Choose where you want to play: \n");

    printf("# 1) Online (Unavailable)\n");

    printf("# 2) Offline, 2 Shells (Unavailable)\n");

    printf("# 3) Offline, 1 Shell\n");

    int choice = 0;

    scanf("%d", &choice);

    switch (choice) {

        case 1:
        case 2:
            printf("# This is unavailable at the time.\n");
            sg_displayWhereToPlay(name);
            break;
        case 3:

            sg_showOffline();

            break;
        default:
            printf("# Number not unavailable.\n");

            sg_displayWhereToPlay(name);
            break;
    }
}

int readTraySize() {

    printf("# What size of tray do you want to play ?\n");

    int traySize = 0;

    scanf(" %d", &traySize);

    return traySize;
}

PossiblePieces *readPieces() {

    printf("# What pieces do you want to use?\n");

    printf("# 1) Default pieces.\n");



}

void sg_showOffline() {

    int traySize = readTraySize();


}
