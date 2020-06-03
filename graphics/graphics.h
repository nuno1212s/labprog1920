#ifndef LABPROG_GRAPHICS_H
#define LABPROG_GRAPHICS_H

#include "../structures/gamestructures.h"
#include "../structures/game.h"
#include "../gameplay.h"

typedef enum {
    SHELL
} GRAPHICS;

/**
 * Initialize the graphics to a certain graphics configuration
 */
void initGraphics(GRAPHICS);

/**
 * Read the possible pieces for a given game from the user
 * @return
 */
PossiblePieces* g_readPossiblePieces(Game *);

/**
 * Read the desired game size from the user
 * @return
 */
int g_readGameSize();

/**
 * Read the player name from the user
 *
 * @return A heap allocated name instance
 */
char *g_readPlayerName();

/**
 * Read a position from the user
 *
 * @return
 */
Position *g_readPosition();

/**
 * Display the size of the game tray to the user
 * @param size
 */
void g_displayGameSize(int size);

/**
 * Display the name of the opponent
 * @param name
 */
void g_displayOpponentName(char *name);

/**
 * Display to the user that he has already played in that position
 */
void g_alreadyPlayedThere();

/**
 * Display to the user that he has missed his shot
 */
void g_missed();

/**
 * Display to the user that his shot has hit a boat
 */
void g_hitBoat();

/**
 * Display to the user that his shot has destroyed a boat
 */
void g_destroyedBoat();

/**
 * Display to the user the pieces that he can place on the board
 * @param placed
 */
void g_showPlaceablePieces(Player *, PossiblePieces *, PieceInBoard **placed);

/**
 * Display to the user that he can't place the piece in the given position
 */
void g_showNotPossibleToPlace(Piece *, Position *);

/**
 * Display to the user that he has successfully placed the piece in the given position
 */
void g_showPiecePlaced(Piece *, Position *);

/**
 * Request the piece that the user wishes to place on the tray
 * @param maxSize The max number to accept
 * @return
 */
int g_requestPieceToPlay(int maxSize);

/**
 * Read the direction that the user wants to place the piece in
 * @return
 */
PlacedDirection g_readPlaceDirection();

/**
 * Display that we are currently for waiting for action from the opponent
 */
void g_waitingForOpponent();

/**
 * Show that it is your turn to choose a place to play
 * @param player
 */
void g_showYourTurn(Player*player);

/**
 * Show that it is the opponents turn to play
 * @param player
 */
void g_showOtherTurn(Player *player);

/**
 * Show that the opponents hit has destroyed one of your boats
 *
 * @param pos
 */
void g_otherPlayerDestroyedBoat(Player *,Position *pos);

/**
 * Show that the opponents hit has hit one of your boats
 * @param pos
 */
void g_otherPlayerHit(Player *, Position *pos);

/**
 * Show that the opponents hit has missed.
 * @param pos
 */
void g_otherPlayerMissed(Player *, Position *pos);

#endif //LABPROG_GRAPHICS_H
