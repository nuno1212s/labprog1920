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

struct Hit_ {

    enum {
        H_DESTROYED_BOAT,
        H_HIT_BOAT,
        H_MISSED,
        H_ALREADY_HIT
    } hitType;

    PieceInBoard *hit;

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

    LinkedList *currentActivePieces;

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
 * @return
 */
Player *initPlayer(char *name, int size);

Player *getCurrentPlayer(Game *);

void goToNextPlayer(Game *);

PieceInBoard *addPieceChosen(Player *, Position *, Piece *, PlacedDirection);

int canPlacePiece(Player *, Position *, Piece *, PlacedDirection);

/**
 * Attempt to play in a position
 *
 * @return The result of the play
 */
Hit playAt(Game *, Player *, Position *);

/**
 * Check if the game has finished
 *
 * @param g The game to check
 * @return -1 if the game has not finished, the index of the player that won if it has
 */
int hasFinished(Game *g);

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

PossiblePieces *getPossiblePieces();

void addPossiblePiece(Piece *piece);

#endif //LABPROG_GAME_H
