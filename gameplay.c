#include "gameplay.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "graphics/graphics.h"
#include "communication/communications.h"

#define PLAYER_COUNT 2

static int playerID;

static int isHost;

static PossiblePieces *readPieces(Game *g);

static void runGame(Game *);

static void placePieces(Game *, Player *);

void startGame(int host) {

    initGraphics(SHELL);

    isHost = host;

    int traySize;

    char *playerName;

    Player *player0, *player1;

    Player **players = malloc(sizeof(Player *) * PLAYER_COUNT);

    initComms(host);

    if (host == 1) {

        playerID = 0;

        c_block();

        traySize = g_readGameSize();

        c_writeGameSize(traySize);

        g_displayGameSize(traySize);

        c_block();

        playerName = g_readPlayerName();

        player0 = initPlayer(playerName, traySize, 1);

        c_sendPlayerInformation(0, player0);

        player1 = initPlayer(NULL, traySize, 0);

        c_readPlayerInformation(1, player1);

        g_displayOpponentName(player1->name);
    } else {

        playerID = 1;

        traySize = c_readGameSize();

        g_displayGameSize(traySize);

        player0 = initPlayer(NULL, traySize, 0);

        c_readPlayerInformation(0, player0);

        c_block();

        playerName = g_readPlayerName();

        player1 = initPlayer(playerName, traySize, 1);

        c_sendPlayerInformation(1, player1);

        g_displayOpponentName(player0->name);
    }

    free(playerName);

    players[0] = player0;
    players[1] = player1;

    Game *game;

    if (host) {
        game = initGame(PLAYER_COUNT, traySize, players);

        c_sendGameInfo(game);
    } else {
        game = initGame(PLAYER_COUNT, traySize, players);

        c_readGameInfo(game);
    }

    printf("The gameID is: %d, Current player is: %s\n", game->gameID, getCurrentPlayer(game)->name);

    sleep(2);

    PossiblePieces *pieces = readPieces(game);

    if (host) {
        placePieces(game, player0);

        player1->currentActivePieceCount = ll_size(pieces->piecesList);
    } else {
        placePieces(game, player1);

        player0->currentActivePieceCount = ll_size(pieces->piecesList);
    }

    runGame(game);

    freeGame(game);

    c_destroy();
}

PossiblePieces *readPieces(Game *game) {

    if (isHost) {

        printf("Reading pieces host.\n");

        c_block();

        PossiblePieces *pieces = g_readPossiblePieces(game);

        c_sendPossiblePieces(pieces);

        return pieces;
    } else {
        PossiblePieces *pieces = c_receivePossiblePieces(game);

        return pieces;
    }

}

void placePieces(Game *game, Player *player) {

    int pieceCount = getPossiblePiecesCount(game->p);

    PieceInBoard ** hasBeenPlaced = calloc(pieceCount, sizeof(PieceInBoard*));

    while (1) {

        g_showPlaceablePieces(player, game->p, hasBeenPlaced);

        int pieceID = g_requestPieceToPlay(pieceCount);

        if (pieceID == -1) {
            if (player->currentActivePieceCount < getPossiblePiecesCount(game->p)) {

                randomizePiecesLeft(player, game->size, (PieceInBoard **) hasBeenPlaced, game->p);

                continue;
            }

            break;
        }

        Piece *piece = getPieceWithId(game->p, pieceID);

        Position *pos = g_readPosition();

        PlacedDirection dir = g_readPlaceDirection();

        PieceInBoard *piecePlaced = addPieceChosen(player, pos, piece, dir);

        if (piecePlaced == NULL) {
            g_showNotPossibleToPlace(piece, pos);
        } else {
            p_free(pos);
            g_showPiecePlaced(piece, pos);

            hasBeenPlaced[pieceID] = piecePlaced;
        }
    }

    g_waitingForOpponent();

    c_waitForOtherPlayerToChoosePieces();


}

void runGame(Game *game) {

    printf("Starting game...\n");

    while (hasFinished(game) == -1) {

        printf("Current player: %d, PlayerID: %d\n", game->currentPlayerIndex, playerID);

        if (game->currentPlayerIndex == playerID) {

            c_block();

            Player *playerData = getCurrentPlayer(game);

            g_showYourTurn(playerData);

            Position *toPlayAt = g_readPosition();

            if (hasPlayedAt(playerData, toPlayAt)) {

                g_alreadyPlayedThere();

                p_free(toPlayAt);

                continue;
            }

            c_sendAttemptedPlay(toPlayAt, game->currentPlayerIndex, game->gameID);

            HitResult result = c_receivedAttemptedPlayResult(game->gameID);

            if (result.playerID != playerID) {
                perror("FAILED TO LOAD RESULT FOR PLAY");

                exit(1);
            }

            registerPlayResult(game, playerData, toPlayAt, result.type);

            p_free(toPlayAt);

            switch (result.type) {

                case H_ALREADY_HIT:
                    g_alreadyPlayedThere();
                    break;
                case H_DESTROYED_BOAT:
                    g_destroyedBoat();
                    break;
                case H_HIT_BOAT:
                    g_hitBoat();
                    break;
                case H_MISSED:
                    g_missed();
                    break;

            }
        } else {
            Player *current = getCurrentPlayer(game);

            printf("Hello\n");

            g_showOtherTurn(current);
            printf("Hello 2\n");

            Played played = c_receiveAttemptedPlay(game->gameID);

            printf("Hello 3\n");
            if (played.playerID != game->currentPlayerIndex) {
                perror("FAILED TO LOAD RESULT FOR ATTEMPTED PLAY");

                exit(1);
            }

            Hit result = playAt(game, current, played.pos);

            c_respondToAttemptedPlay(played.playerID, result.hitType, game->gameID);
        }

    }

}