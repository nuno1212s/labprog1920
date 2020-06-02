#include "same_shell.h"
#include "../graphics/graphics.h"
#include <stdio.h>
#include <stdlib.h>

static Game *game;

static int currentGameSize;

static PossiblePieces *pieces;

void ssh_init() {
}

int ssh_readGameSize() {
    return currentGameSize;
}

void ssh_writeGameSize(int size) {
    currentGameSize = size;
}

void ssh_readPlayerInformation(Player * player) {
    char *pName = g_readPlayerName();

    pl_setname(player, pName);

    player->storage = initGameStorage(currentGameSize);
}

void ssh_sendGameInfo(Game *g) {
    game = g;
}

void ssh_sendPlayerInformation(Player *p) {}

void ssh_sendPossiblePieces(PossiblePieces *p) {
    pieces = p;
}

PossiblePieces *ssh_receivePossiblePieces(Game *game) {
    return pieces;
}

void ssh_waitForOtherPlayerToChoosePieces() {

    Player *notChosenYet = NULL;

    for (int i = 0; i < game->playerCount; i++) {
        if (pl_activePieces(game->players[i]) == 0) {
            notChosenYet = game->players[i];
            break;
        }
    }

    printf("Hello\n");

    int pieceCount = getPossiblePiecesCount(game->p);

    PieceInBoard ** hasBeenPlaced = calloc(pieceCount, sizeof(PieceInBoard*));

    while (1) {

        g_showPlaceablePieces(notChosenYet, game->p, hasBeenPlaced);

        int pieceID = g_requestPieceToPlay(pieceCount);

        if (pieceID == -1) {
            if (pl_activePieces(notChosenYet) < getPossiblePiecesCount(game->p)) {

                randomizePiecesLeft(notChosenYet, game->size, (PieceInBoard **) hasBeenPlaced, game->p);

                continue;
            }

            break;
        }

        Piece *piece = getPieceWithId(game->p, pieceID);

        Position *pos = g_readPosition();

        PlacedDirection dir = g_readPlaceDirection();

        PieceInBoard *piecePlaced = addPieceChosen(notChosenYet, pos, piece, dir);

        if (piecePlaced == NULL) {
            g_showNotPossibleToPlace(piece, pos);
        } else {
            p_free(pos);
            g_showPiecePlaced(piece, pos);

            hasBeenPlaced[pieceID] = piecePlaced;
        }
    }
}

HitResult result;

void ssh_sendAttemptedPlay(Position *pos, int playerID, int gameID) {

    Player *current = getCurrentPlayer(game);

    Hit hResult = playAt(game, current, pos);

    result.type = hResult.hitType;
    result.playerID = playerID;

}

HitResult ssh_receivedAttemptedPlayResult(int gameID) {
    return result;
}

Position *toPlayAt;

Player *playerData;

Played ssh_receiveAttemptedPlay(int gameID) {

    playerData = getCurrentPlayer(game);

    g_showYourTurn(playerData);

    toPlayAt = g_readPosition();

    if (hasPlayedAt(playerData, toPlayAt)) {

        g_alreadyPlayedThere();

        p_free(toPlayAt);

        return ssh_receiveAttemptedPlay(gameID);
    }

    Played played;

    played.pos = toPlayAt;
    played.playerID = game->currentPlayerIndex;
    played.gameID = gameID;

    return played;
}

void ssh_respondToAttemptedPlay(int playerID, HitType hit, int gameID) {
    registerPlayResult(game, playerData, toPlayAt, hit);

    p_free(toPlayAt);


    switch (hit) {

        case H_ALREADY_HIT:
            g_alreadyPlayedThere();
            printf("ALREADY HIT Current player Before %s\n" , pl_name(getCurrentPlayer(game)));
            break;
        case H_DESTROYED_BOAT:
            g_destroyedBoat();
            printf("DESTROYED BOAT Current player Before %s\n" , pl_name(getCurrentPlayer(game)));
            break;
        case H_HIT_BOAT:
            g_hitBoat();
            printf("HIT_BOAT Current player Before %s\n" , pl_name(getCurrentPlayer(game)));
            break;
        case H_MISSED:

            //We increment the player counter one last time as it gets incremented twice
            //So it resets to the base player
            goToNextPlayer(game);

            g_missed();
            break;
    }

}
