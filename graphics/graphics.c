
#include "graphics.h"
#include "shell2_0.h"

static GRAPHICS graphicsType;

void initGraphics(GRAPHICS type) {
    graphicsType = type;
}

GRAPHICS getGraphics() {
    return graphicsType;
}

GAME_TYPE g_readGameType() {

    switch (graphicsType) {

        case SHELL:
            return sh_readGameType();

    }

}

PossiblePieces *g_readPossiblePieces(Game *game) {
    switch (graphicsType) {

        case SHELL:
            return sh_readPossiblePieces(game);

    }
}

int g_readGameSize() {
    switch (graphicsType) {

        case SHELL:
            return sh_readGameSize();

    }
}

char *g_readPlayerName() {
    switch (graphicsType) {

        case SHELL:
            return sh_readPlayerName();

    }
}

Position *g_readPosition() {

    switch (graphicsType) {

        case SHELL:
            return sh_readPosition();

    }
}

void g_alreadyPlayedThere() {

    switch (graphicsType) {
        case SHELL:
            sh_alreadyPlayedThere();
            break;
    }

}

void g_missed() {
    switch (graphicsType) {
        case SHELL:
            sh_missed();
            break;
    }
}

void g_hitBoat() {
    switch (graphicsType) {
        case SHELL:
            sh_hitBoat();
            break;
    }
}

void g_destroyedBoat() {
    switch (graphicsType) {

        case SHELL:
            sh_destroyedBoat();
            break;

    }
}

void g_showPlaceablePieces(Player *player, PossiblePieces *pieces, PieceInBoard **placed) {

    switch (graphicsType) {

        case SHELL:
            sh_showPlaceablePieces(player, pieces, placed);
            break;
    }
}

void g_showNotPossibleToPlace(Piece *piece, Position *pos) {

    switch (graphicsType) {
        case SHELL:
            sh_showNotPossibleToPlace(piece, pos);
            break;
    }
}

void g_showPiecePlaced(Piece *piece, Position *pos) {

    switch (graphicsType) {
        case SHELL:
            sh_showPiecePlaced(piece, pos);
            break;
    }
}

int g_requestPieceToPlay(int maxSize) {

    switch (graphicsType) {
        case SHELL:
            sh_requestPieceToPlay(maxSize);
            break;
    }
}

PlacedDirection g_readPlaceDirection() {

    switch (graphicsType) {
        case SHELL:
            return sh_readPlaceDirection();
    }
}

void g_showYourTurn(Player *player) {

    switch (graphicsType) {
        case SHELL:
            sh_showYourTurn(player);
            break;
    }
}

void g_showOtherTurn(Player *player) {

    switch (graphicsType) {
        case SHELL:
            sh_showOtherTurn(player);
            break;
    }
}