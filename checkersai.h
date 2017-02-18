#include "checkers.h"

/*
 * A move is represented by the tuple (row, col, dir), where
 * - row and col are 3-bit unsigned integers representing a point on the board
 * - dir is a member of enum dir from checkers.h
 */
typedef unsigned char move;
#define MOVE_ROW(m) ((m) >> 5)
#define MOVE_COL(m) ((m) >> 2 & 7)
#define MOVE_DIR(m) ((m) & 3)

/*
 * Make optimal move, as calculated by optimal_move
 * Return error code of checkers_play
 * Negative result means no valid move, game lost
 */
int play_optimal_move(struct checkers *);

/* 
 * Calculates optimal move for current player through alpha-beta pruning
 * Returns an element of enum dir, or -1 if no valid move
 */
move optimal_move(struct checkers *);

/* 
 * Applies alpha-beta pruning to board with given maximum depth, lower and upper bounds and current player
 * Returns best heuristic value and the move that gives rise to it
 * Initially called as alphabeta(&c, depth, INT_MIN, INT_MAX, c->turn, &best); 
 */
int alphabeta(struct checkers *, int depth, int alpha, int beta, int player, move *best);

/*
 * Assigns to int array all moves on the board
 * Returns size of array
 */
int moves(struct checkers *, move *);
