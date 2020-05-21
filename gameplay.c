#include "gameplay.h"

#include <stdlib.h>
#include <stdio.h>
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

    int traySize;

    char *playerName;

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

        playerName = g_readPlayerName();

        player0 = initPlayer(playerName, traySize, 1);

        c_sendPlayerInformation(player0);

        player1 = initPlayer(NULL, traySize, 0);

        c_readPlayerInformation(player1);

    } else {

        playerID = 1;

        traySize = c_readGameSize();

        player0 = initPlayer(NULL, traySize, 0);

        c_readPlayerInformation(player0);

        playerName = g_readPlayerName();

        player1 = initPlayer(playerName, traySize, 1);

        c_sendPlayerInformation(player1);
    }

    free(playerName);

    players[0] = player0;
    players[1] = player1;

    Game *game = initGame(PLAYER_COUNT, traySize, players);

    PossiblePieces *pieces = readPieces(game);

    placePieces(game, host ? player0 : player1);

    runGame(game);

    freeGame(game);
}

PossiblePieces *readPieces(Game *game) {

    if (isHost) {

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

    PieceInBoard *hasBeenPlaced[pieceCount];

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

        Piece *piece = getPieceWithId(pieceID);

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

    c_waitForOtherPlayerToChoosePieces();
}

void runGame(Game *game) {

    while (!hasFinished(game)) {

        if (game->currentPlayerIndex == playerID) {

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

            g_showOtherTurn(current);

            Played played = c_receiveAttemptedPlay(game->gameID);

            if (played.playerID != game->currentPlayerIndex) {
                perror("FAILED TO LOAD RESULT FOR ATTEMPTED PLAY");

                exit(1);
            }

            Hit result = playAt(game, current, played.pos);

            c_respondToAttemptedPlay(played.playerID, result.hitType, game->gameID);
        }

    }

}