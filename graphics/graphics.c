
#include "graphics.h"
#include "shell.h"

static GRAPHICS graphicsType;

void initGraphics(GRAPHICS type) {
    graphicsType = type;
}

GRAPHICS getGraphics() {
    return graphicsType;
}

void displayMenu() {

    switch (getGraphics()) {

        case SHELL: {

            sg_displayMenu();

            break;
        }

    }

}