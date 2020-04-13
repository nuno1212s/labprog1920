
#include "graphics.h"
#include "shell.h"

static GRAPHICS graphicsType;

void initGraphics(GRAPHICS type) {
    graphicsType = type;
}

GRAPHICS getGraphics() {
    return graphicsType;
}

Game* displayMenu() {

    switch (getGraphics()) {

        case SHELL: {

            return sg_displayMenu();

        }

    }

}

void displayGame(Game *g) {

    switch (getGraphics()) {
        case SHELL:
            sg_displayGame(g);
            break;
    }

}