#include "checkersai.h"

int play_optimal_move(struct checkers *c)
{
    move best = optimal_move(c);
    int row = MOVE_ROW(best), col = MOVE_COL(best);
    return checkers_play(c, &row, &col, MOVE_DIR(best));
}

move optimal_move(struct checkers *c)
{
    move best;
    alphabeta(c, MAX_DEPTH, INT_MIN, INT_MAX, c->player, &best);
    return best;
}

int alphabeta(struct checkers *c, int depth, int alpha, int beta, int player, move *best)
{
    if (!depth || c->scores[0] == 12 || c->scores[1] == 12)
        return 2 * c->scores[player] - c->scores[!player]; // favors scoring more
    int v, v1, ptc;
    move pts[MAX_MOVES], m;
    struct checkers c1;
    
    v = player == c->player ? INT_MIN : INT_MAX;
    
    // iterate through each valid (row, col) that can move
    ptc = moves(c, pts);
    for (int i = 0; i < ptc; ++i) {
        c1 = *c; // should this assignment work?
        !checkers_play(&c1, MOVE_ROW(pts[i]), MOVE_COL(pts[i]), MOVE_DIR(pts[i]));
        v1 = alphabeta(&c1, depth - 1, alpha, beta, player, &m);
        
        if ((player == c->player ? v < v1 : v > v1) || ((rand() & 1) && v == v1)) { // randomized tie breaking
            v = v1;
            *best = pts[i];
        }
        if (player == c->player && alpha < v) 
            alpha = v;
        if (player != c->player && beta > v) 
            beta = v;
        if (alpha >= beta)
            break;
    }
    return v;
}

// this may perhaps also calculate direction, in which case alphabeta doesn't need to check play again
int moves(struct checkers *c, move *pts)
{
    int ptc = 0, row, col;
    for (int pt = 0; pt < 256; ++pt) {
        row = MOVE_ROW(pt);
        col = MOVE_COL(pt);
        if (ptc < MAX_MOVES && movable(c, &row, &col, MOVE_DIR(pt)) > 0) {
            pts[ptc++] = pt;
        }
    }
    return ptc;
}
