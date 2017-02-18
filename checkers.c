#include "checkers.h"

void checkers_init(struct checkers *c)
{
    int i, j;
    
    if (!c)
        return;

    c->player = c->turn = c->scores[0] = c->scores[1] = 0;
    for (i = 0; i < 3; ++i) {
        for (j = i & 1; j < 8; j += 2) {
            c->table[i][j]     = PL1;
            c->table[7-i][7-j] = PL2;
        }
    }
}

int checkers_play(struct checkers *c, int *row, int *col, int dir)
{
    int row1 = *row, col1 = *col, mov = movable(c, &row1, &col1, dir);
    if (mov <= 0)
        return mov;
    
    c->table[row1][col1] = c->table[*row][*col];
    c->table[*row][*col] = EMPTY;
    c->turn++;
    
    if (mov == SCORED) {
        c->table[(*row + row1)/2][(*col + col1)/2] = EMPTY;
        c->scores[c->player]++;
        // TODO should only be able to move the current puck, and toggle player if the cursor is let go
    }
    if (row1 == 7 * (1 - c->player)) { // becomes king
        c->table[row1][col1] |= 4;
    }
    c->player ^= 1;
    *row = row1;
    *col = col1;
    
    return mov;
}

int movable(struct checkers *c, int *row, int *col, int dir)
{
    if (!c || !row || !col)
        return 0;
    
    int row1 = *row, col1 = *col, drow, dcol;
    if (row1 & 8 || col1 & 8)
        return OUTOFBOUNDS;
    
    int puck = c->table[row1][col1];
    if (!(puck & 1) || (puck & 2 ^ (c->player << 1))) // puck not occupied or occupied by another player
        return INVALID;
    if (!(puck & 4) && dir & 2) // puck not king but attempting to move backwards
        return DIRBLOCKED; 
    
    drow = BOOLTOSIGN(dir & 2) * BOOLTOSIGN(puck & 2);
    dcol = BOOLTOSIGN(dir & 1) * -BOOLTOSIGN(puck & 2); // move -1 if left, 1 if right
    row1 += drow;
    col1 += dcol;
    
    // new position out of bounds
    if (row1 < 0 || row1 > 7 || col1 < 0 || col1 > 7)
        return DIRBLOCKED;
    // new position occupied
    if (c->table[row1][col1]) {
        // can skip puck in new position (occupied by another player and the next position is not occupied)
        if (!((row1+drow) & 8) && !((col1+dcol) & 8) &&  
            (c->table[row1][col1] ^ puck) & 2 && !c->table[row1+drow][col1+dcol]) {
            *row = row1 + drow;
            *col = col1 + dcol;
            return SCORED;
        } else {
            return DIRBLOCKED;
        }
    }
    
    *row = row1;
    *col = col1;
    return REGULAR;
}

