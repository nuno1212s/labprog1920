#include <stdio.h>

#include "graphics/graphics.h"

#include "storagestructures/bitmap.h"

int main() {
    printf("Hello, World!\n");

    Game* currentGame = displayMenu();

    freeGame(currentGame);

    return 0;
}
