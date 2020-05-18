#include "gameplay.h"

#include <stdlib.h>
#include <stdio.h>
#include "graphics/graphics.h"
#include "communication/communications.h"

#define PLAYER_COUNT 2

static int playerID;

static int isHost;

static void readPieces();

static void runGame(Game *);

static void placePieces(Game *);

void startGame(int host) {

    initGraphics(SHELL);

    int traySize;

    char *player0Name, *player1Name;

    Player *player0, *player1;

    Player **players = malloc(sizeof(Player *) * PLAYER_COUNT);

    if (host == 1) {
        GAME_TYPE type = g_readGameType();

        switch (type) {
            case ONE_SHELL:
                initComms(SAME_SHELL, host);
                break;
            case TWO_SHELL_TEXT:
                initComms(TEXT, host);
                break;
            case TWO_SHELL_PIPES:
                initComms(PIPES, host);
                break;
            case TWO_SHELL_NETWORK:
                initComms(NETWORK, host);
                break;
        }

        playerID = 0;

        traySize = g_readGameSize();

        player0Name = g_readPlayerName();

        player0 = initPlayer(player0Name, traySize, 1);

        c_sendPlayerInformation(player0);

        player1 = initPlayer(NULL, traySize, 0);

        c_readPlayerInformation(player1);

    } else {

        playerID = 1;

        traySize = c_readGameSize();

        player0 = initPlayer(NULL, traySize, 0);

        c_readPlayerInformation(player0);

        player1Name = g_readPlayerName();

        player1 = initPlayer(player1Name, traySize, 1);

        c_sendPlayerInformation(player1);
    }

    players[0] = player0;
    players[1] = player1;

    readPieces();

    Game *game = initGame(PLAYER_COUNT, traySize, players);

    placePieces(game);

    runGame(game);

    freeGame(game);
}

void readPieces() {

    if (isHost) {

        PossiblePieces *pieces = g_readPossiblePieces();

        setPossiblePieces(pieces);

        c_sendPossiblePieces(pieces);

    } else {
        PossiblePieces *pieces = c_receivePossiblePieces();

        setPossiblePieces(pieces);
    }

}

void runGame(Game *game) {

    while (!hasFinished(game)) {

        if (game->currentPlayerIndex == playerID) {

            Player *playerData = getCurrentPlayer(game);

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

            registerPlayResult(game, playerData, toPlayAt, result.type);

            p_free(toPlayAt);

        } else {

            Played played = c_receiveAttemptedPlay(game->gameID);

            if (played.playerID != game->currentPlayerIndex) {
                perror("FAILED TO LOAD RESULT FOR ATTEMPTED PLAY");

                exit(1);
            }

            Player *current = getCurrentPlayer(game);

            Hit result = playAt(game, current, played.pos);

            c_respondToAttemptedPlay(played.playerID, result.hitType, game->gameID);

        }

    }

}