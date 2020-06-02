#ifndef LABPROG_GAME_H
#define LABPROG_GAME_H

#include "position.h"
#include "gamestructures.h"
#include "../storagestructures/linkedlist.h"
#include "gamepieces.h"

#define g_gameID(g) (g->gameID)
#define g_gameSize(g) (g->size)
#define g_currentPlayer(g) (g->currentPlayerIndex)
#define g_possiblePieces(g) (g->p)

#define pl_name(p) (p->name)
#define pl_setname(p, newName) (p->name = newName)
#define pl_storage(p) (p->storage)
#define pl_activePieces(p) (p->currentActivePieceCount)
#define pl_setActivePieces(p, pieces) (p->currentActivePieceCount = pieces)

#define MIN_GAME_SIZE 20
#define MAX_GAME_SIZE 40
#define MAX_PLAYER_NAME_SIZE 24

typedef enum {
    H_DESTROYED_BOAT,
    H_HIT_BOAT,
    H_MISSED,
    H_ALREADY_HIT
} HitType;

struct Hit_ {

    HitType hitType;

};

struct Game_ {

    int gameID;

    int playerCount;

    int size;

    struct Player_ **players;

    int currentPlayerIndex;

    PossiblePieces *p;

};

struct Player_ {

    char *name;

    GameStorage *storage;

    int currentActivePieceCount;

};


typedef struct Player_ Player;

typedef struct Game_ Game;

typedef struct Hit_ Hit;


/**
 * Initialize a game instance with the given attributes
 *
 * @param playerCount The amount of players
 * @param traySize The size of the game tray
 * @param playerList The list of players
 * @return
 */
Game *initGame(int playerCount, int traySize, Player **playerList);

void freeGame(Game *);

/**
 * Creates a player object
 *
 * @param name The name of the player. This parameter is duplicated, so it can be safely freed afterwards
 * @param size The size of the game tray
 * @param isLocal Whether this is the local player instance
 * @return
 */
Player *initPlayer(char *name, int size, int isLocal);

Player *getPlayer(Game *, int);

Player *getCurrentPlayer(Game *);

Player *getOtherPlayer(Game *);

/**
 * Move the turn to the next player
 */
void goToNextPlayer(Game *);

/**
 * Register a piece that the player has tried to place in the given position and direction
 * @return
 */
PieceInBoard *addPieceChosen(Player *, Position *, Piece *, PlacedDirection);

/**
 * Check if a player can place a given piece in a given position with a given direction
 * @return
 */
int canPlacePiece(Player *, Position *, Piece *, PlacedDirection);

/**
 * Check whether the player has played in the given position
 * @return
 */
int hasPlayedAt(Player *, Position *);

/**
 * Register an attempt by the player to play in the given position
 *
 * This method will go to the opposite player and check whether the player has hit any boats.
 *
 * @return The result of the play
 */
Hit playAt(Game *, Player *, Position *);

/**
 * Register the result of a play made by the player in the given position with given hit type
 *
 * This method will store the result of the play in the players storage
 *
 * @param type
 */
void registerPlayResult(Game *,Player *, Position *, HitType type);

/**
 * Check if the game has finished
 *
 * @param g The game to check
 * @return -1 if the game has not finished, the index of the player that won if it has
 */
int hasFinished(Game *g);

/**
 * Randomly place the pieces that the player has not yet placed
 * @param size
 */
void randomizePiecesLeft(Player *, int size, PieceInBoard **, PossiblePieces *);

/**
 * Free the given player object
 */
void freePlayer(Player *);

#endif //LABPROG_GAME_H
