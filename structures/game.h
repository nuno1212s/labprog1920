#ifndef LABPROG_GAME_H
#define LABPROG_GAME_H

#include "position.h"
#include "gamestructures.h"
#include "../storagestructures/linkedlist.h"

#define g_gameID(g) (g->gameID)
#define g_gameSize(g) (g->size)
#define g_currentPlayer(g) (g->currentPlayerIndex)

struct PossiblePieces_ {

    LinkedList *piecesList;

};

typedef enum {
    H_DESTROYED_BOAT,
    H_HIT_BOAT,
    H_MISSED,
    H_ALREADY_HIT
} HitType;

struct Hit_ {

    HitType hitType;

};

struct SearchingForGame_ {

    struct PossiblePieces_ *pieces;

    struct Player_ *player;

    int size;

};

struct Game_ {

    int gameID;

    int playerCount;

    int size;

    struct Player_ **players;

    int currentPlayerIndex;

};

struct Player_ {

    char *name;

    GameStorage *storage;

    int currentActivePieceCount;

};

typedef struct PossiblePieces_ PossiblePieces;

typedef struct Player_ Player;

typedef struct Game_ Game;

typedef struct SearchingForGame_ SearchingForGame;

typedef struct Hit_ Hit;

Game *initGame(int, int, Player **);

void freeGame(Game *);

/**
 * Creates a player object
 *
 * @param name The name of the player. This parameter is duplicated, so it can be safely freed afterwards
 * @param size The size of the game tray
 * @param isHost Whether this is the local player instance
 * @return
 */
Player *initPlayer(char *name, int size, int isHost);

Player *getCurrentPlayer(Game *);

void goToNextPlayer(Game *);

PieceInBoard *addPieceChosen(Player *, Position *, Piece *, PlacedDirection);

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

void randomizePiecesLeft(Player *, int size, PieceInBoard **, PossiblePieces *);

SearchingForGame *randomizePieces(Player *, int size);

SearchingForGame *initGameForPlayer(Player *, int size);

void freePlayer(Player *);

SearchingForGame *initSearchGame(PossiblePieces *, int, Player *);

/**
 * Deleting the Search game does not delete the player objects
 *
 * So the player object from this searching game can be safely used in the game object
 */
void deleteSearchGame(SearchingForGame *);

void setPossiblePieces(PossiblePieces *);

PossiblePieces *initPossiblePieces();

void createDefaultPossiblePieces(int size);

PossiblePieces *getPossiblePieces();

void addPossiblePiece(Piece *piece);

#endif //LABPROG_GAME_H
