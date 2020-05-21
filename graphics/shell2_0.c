#include "shell2_0.h"

#include <stdio.h>
#include <stdlib.h>

static void showPlayerTray(Player *player);

int sh_readGameSize() {
    system("clear");

    printf("Please insert the size of game you want to play. (Between %d and %d)\n", MIN_GAME_SIZE, MAX_GAME_SIZE);

    int gameSize;

    scanf("%d", &gameSize);

    if (gameSize < MIN_GAME_SIZE || gameSize > MAX_GAME_SIZE) {
        return sh_readGameSize();
    }

    return gameSize;
}

char *sh_readPlayerName() {
    system("clear");

    char *playerNameBuffer = malloc(sizeof(char) * MAX_PLAYER_NAME_SIZE);

    printf("Insert your name here: (Max of %d characters)\n", MAX_PLAYER_NAME_SIZE);

    scanf("%24s", playerNameBuffer);

    return playerNameBuffer;
}

Position *sh_readPosition() {

    system("clear");

    printf("Insert a position: \n");

    int x, y;

    printf("X: ");
    scanf("%d\n", &x);

    printf("Y:");
    scanf("%d", &y);

    Position *pos = initPos(x, y);

    return pos;
}

void sh_alreadyPlayedThere() {

    printf("You have already played in this position!\n");

}

void sh_missed() {

    printf("Your shot has not hit any boats.\n");

}

void sh_hitBoat() {

    printf("Your shot has hit a boat!\n");

}

void sh_destroyedBoat() {
    printf("Your shot has destroyde a boat!\n");
}

void sh_showPlaceablePieces(Player *game, PossiblePieces *piece, int *placed) {

    showPlayerTray(game);

    int pieceCount = getPossiblePiecesCount(piece);

    int placedPieces = 0;

    for (int i = 0; i < pieceCount; i++) {

        printf("%d) %s", i, game->name);

        if (placed[i]) {
            placedPieces++;
            printf(" - (Placed)\n");
        } else {
            printf("\n");
        }

    }

}

void sh_showNotPossibleToPlace(Piece *, Position *);

void sh_showPiecePlaced(Piece *, Position *);

void sh_showAllPlaced();

int sh_requestPieceToPlay();

PlacedDirection sh_readPlaceDirection();