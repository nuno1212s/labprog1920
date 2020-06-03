
#include "graphics.h"
#include "shell.h"

static GRAPHICS graphicsType;

void initGraphics(GRAPHICS type) {
    graphicsType = type;
}

GRAPHICS getGraphics() {
    return graphicsType;
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

void g_waitingForOpponent() {
    switch (graphicsType) {
        case SHELL:
            sh_waitingForOpponent();
            break;
    }
}

void g_displayGameSize(int gameSize) {

    switch (graphicsType) {
        case SHELL:
            sh_displayGameSize(gameSize);
            break;
    }

}

void g_displayOpponentName(char *name) {
    switch (graphicsType) {
        case SHELL:
            sh_displayOpponentName(name);
            break;
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

void g_otherPlayerDestroyedBoat(Player *player, Position *pos) {

    switch (graphicsType) {
        case SHELL:
            sh_otherPlayerDestroyedBoat(player, pos);
            break;
    }

}

void g_otherPlayerHit(Player *player, Position *pos) {

    switch (graphicsType) {
        case SHELL:
            sh_otherPlayerHit(player, pos);
            break;
    }

}

void g_otherPlayerMissed(Player *player, Position *pos) {

    switch (graphicsType) {
        case SHELL:
            sh_otherPlayerMissed(player, pos);
            break;
    }
}