#ifndef LABPROG_GAME_H
#define LABPROG_GAME_H

#include "position.h"
#include "gamestructures.h"

struct Player_ {

    char *name;

    int id;

};

struct GamePiece_ {

    int size;

    char *name;

};

struct PlayedGamePiece_ {

    enum {
        OWN_PIECE,
        PLAYED_PIECE
    } pieceType;

    struct {
        struct GamePiece_ *piece;

        int hit;
    };

    Position **position;

};

struct Game_ {

    int playerCount;

    struct Player_ **players;

    GameStorage *ourGame, *opponentGame;

};

typedef struct Player_ Player;

typedef struct Game_ Game;

typedef struct GamePiece_ GamePiece;

typedef struct PlayedGamePiece_ PlayedGamePiece;

GamePiece* initGamePiece(int size, char* name);

int g_getPieceSize(GamePiece *);

char* g_getPieceName(GamePiece *);

/**
 * Frees the GamePiece and all of it's components
 */
void g_freePiece(GamePiece *);

PlayedGamePiece* initPlayedPiece(GamePiece *, Position *);

PlayedGamePiece* initAttemptPiece(int hit, Position *);

GamePiece * g_getGamePiece(PlayedGamePiece *);

int g_isHit(PlayedGamePiece *);

Position * g_getPlayedPosition(PlayedGamePiece *);

/**
 * Free the PlayedGamePiece
 *
 * This will free the played piece object and it's position,
 * But it will not destroy the game piece object and player object
 * that is stored within it
 */
void g_freePlayed(PlayedGamePiece *);


#endif //LABPROG_GAME_H
