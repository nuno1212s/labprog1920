#ifndef LABPROG_COMMUNICATIONS_H
#define LABPROG_COMMUNICATIONS_H

#include "../structures/game.h"

//The defines for possible data methods are: SAME_SHELL, TEXT, SHARED_MEM and NETWORK

typedef struct {

    Position *pos;
    int playerID;
    int gameID;

} Played;

typedef struct {

    int playerID;

    HitType type;

} HitResult;

/**
 * Initialize the comms
 *
 * @param host Whether this process is the parent process
 */
void initComms(int host);

/**
 * Block the other client from doing anything (only required on certain modes)
 */
void c_block();

/**
 * Read the same of the game (Determined by the "host")
 * @return
 */
int c_readGameSize();

/**
 * Write the size of the game
 */
void c_writeGameSize(int);

/**
 * Read the player information for a player with the given id
 * @param id
 */
void c_readPlayerInformation(int id, Player *);

/**
 * Send the player information for a player with the given id
 * @param id
 */
void c_sendPlayerInformation(int id, Player *);

/**
 * Send the possible pieces of a game
 */
void c_sendPossiblePieces(PossiblePieces *);

/**
 * Receive all the possible pieces of a game and stores them in the given game parameter
 * @param game
 * @return
 */
PossiblePieces *c_receivePossiblePieces(Game *game);

/**
 * Wait for the other player to choose where he wants to place his pieces
 */
void c_waitForOtherPlayerToChoosePieces();

/**
 * Send all the necessary information about the game to the other player
 */
void c_sendGameInfo(Game *);

/**
 * Receive all necessary information about the game from the other player
 */
void c_readGameInfo(Game *);

/**
 * Send an attempted play, made by us to the other player
 * @param pos The position we tried to play
 * @param playerID The player ID of the player that tried to play
 * @param gameID The ID of the game
 */
void c_sendAttemptedPlay(Position *pos, int playerID, int gameID);

/**
 * Receive the attempted play of the opponent
 *
 * @param gameID The ID of the game
 * @return
 */
Played c_receiveAttemptedPlay(int gameID);

/**
 * Respond to the play that has the other player has made
 * @param playerID
 * @param hit
 * @param gameID
 */
void c_respondToAttemptedPlay(int playerID, HitType hit, int gameID);

/**
 * Receive the result of the play that we have attempted
 *
 * @param gameID
 * @return
 */
HitResult c_receivedAttemptedPlayResult(int gameID);

void c_destroy();

#endif //LABPROG_COMMUNICATIONS_H
