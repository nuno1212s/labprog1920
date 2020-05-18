//
// Created by nuno on 18/05/20.
//

#ifndef LABPROG_GAMEPLAY_H
#define LABPROG_GAMEPLAY_H

typedef enum {
    ONE_SHELL,
    TWO_SHELL_TEXT,
    TWO_SHELL_PIPES,
    TWO_SHELL_NETWORK

} GAME_TYPE;

void startGame(int host);

#endif //LABPROG_GAMEPLAY_H
