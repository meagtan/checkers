#define BOOLTOSIGN(b) ((b) ? 1 : -1) /* convert (0,1) bool to (-1,1) */

enum player {
    P1,
    P2
};

/* The values of the enum constants are determined such that
 * - The 0th bit determines whether the puck is occupied,
 * - The 1st bit determines whether it is occupied by player 1 or 2,
 * - The 2nd bit determines whether it is king, i.e. can move in both directions.
 */
enum puck {
    EMPTY,
    PL1,
    PL2 = 0b11,
    PL1KING = 0b101,
    PL2KING = 0b111
};

/* The 0th bit represents left or right,
   the 1st bit represents backward or forward movement */
enum dir {
    LEFT,
    RIGHT,
    BACKLEFT,
    BACKRIGHT
};

enum mov {
    OUTOFBOUNDS = -4,
    INVALID,
    DIRBLOCKED,
    REGULAR = 1,
    SCORED
};

/* perhaps include cursor */
struct checkers {
    unsigned char player, table[8][8]; /* indexed row first */
    int turn, scores[2];
};

void checkers_init(struct checkers *);

/* The return type encodes different conditions, such as
   - (row,col) out of bounds
   - (row,col) not on puck of player 
   - cannot move in given direction
     - direction out of bounds
     - direction occupied by another puck
   - move scores a puck (should provide option to skip)
   Perhaps span these between -4 and 2 (1 being an OK move, 2 a move that scores) */
int checkers_play(struct checkers *, int *row, int *col, int dir);

/* Test if (row,col) can move in the direction of dir, if true modify (row,col) in that direction
   True if > 0 */
int movable(struct checkers *, int *row, int *col, int dir);
