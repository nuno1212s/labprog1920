#include <stdio.h>

#include "graphics/graphics.h"

#include "storagestructures/bitmap.h"

int main() {
    printf("Hello, World!\n");

    Game* currentGame = displayMenu();

    displayGame(currentGame);

    freeGame(currentGame);

    return 0;
}
