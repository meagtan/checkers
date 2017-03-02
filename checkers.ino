/*
 * Checkers displayed on LED matrix.
 * 
 * Configuration:
 * - Digital output pins 8, 9 and 10 connect to the prongs DIN, CS and CLK of the LED matrix.
 * - Pins 2, 3, 4 and 5 take input from push buttons, connected to 5V and a 10K pull-down resistor.
 *   Pin 2 moves left, pin 3 moves right, pin 4 moves backwards and pin 5 selects a puck.
 */

extern "C" {
    #include "checkersai.h"
}

#define TOROIDAL 1

// buttons
enum {
    LEFTB = 2,
    RIGHTB,
    BACKB,
    SELECTB
};

const int INIT_COUNT = 24, DELAY = 8, PL2BLINK = 180, CURSORBLINK = 100, PERIOD = 200;
const bool automatic = true; // whether to make it play against itself

int row = 3, col = 2, buttons[4][2] = {LOW};
unsigned long curMillis, preMillis = 0, curMil2, preMil2 = 0, preMil3 = 0;
struct checkers game;
bool selected = false, pl2Blinking = false, cursorBlinking = false, calced = false, reset = false;
move best;

// LED matrix port definitions
int Max7219_pinCLK = 10;
int Max7219_pinCS = 9;
int Max7219_pinDIN = 8;

void setup() {
    pinMode(Max7219_pinCLK, OUTPUT);
    pinMode(Max7219_pinCS, OUTPUT);
    pinMode(Max7219_pinDIN, OUTPUT);
    pinMode(LEFTB, INPUT);
    pinMode(RIGHTB, INPUT);
    pinMode(BACKB, INPUT);
    pinMode(SELECTB, INPUT);
    delay(50);
    
    checkers_init(&game);
    
    Init_MAX7219();
    Serial.begin(9600);
    preMil2 = millis();
    srand(analogRead(0)); // seed rand with analog noise
}

void loop() {
    int i, j, n;

    curMil2 = millis();
    if (reset && curMil2 - preMil3 > 2000) {
        Serial.println(game.scores[0]);
        Serial.println(game.scores[1]);
        checkers_init(&game);
        reset = false;
        preMil2 = millis();
    }

    if (!reset && game.player | automatic) {
        //Serial.println(play_optimal_move(&game));

        // play move after delay
        curMil2 = millis();
        if (calced && curMil2 - preMil2 > 1000) {
            int row = MOVE_ROW(best), col = MOVE_COL(best), res;
            Serial.println(res = checkers_play(&game, &row, &col, MOVE_DIR(best)));
            calced = false;
            if (res <= 0) {
                reset = true;
                preMil3 = millis();
            }
            if (res == 2)
                Serial.println(game.player);
        } else if (!calced) {
            best = optimal_move(&game);
            Serial.println(best);
            preMil2 = millis();
            calced = true;
        }
    } else if (selected) {
        // if select is pressed again
        if (buttonPressed(SELECTB))
            selected = false;

        // move puck
        // TODO doesn't register back presses (due to reading, or due to movable?)
        if (buttonPressed(LEFTB)) {
            if (checkers_play(&game, &row, &col, digitalRead(BACKB) == HIGH ? BACKLEFT : LEFT) > 0)
                selected = false;
        }
        if (buttonPressed(RIGHTB)) {
            if (checkers_play(&game, &row, &col, digitalRead(BACKB) == HIGH ? BACKRIGHT : RIGHT) > 0)
                selected = false;
        }

        // TODO modify this so that the player doesn't change after scoring and pressing SELECTB
        // or making a nonscoring move skips to the next player
        
    } else {
        // move cursor
        if (buttonPressed(LEFTB)) {
            if (col) { 
                --col;
            } else if (row != 7) {
                ++row;
                col = 7;
            } else {
                row = 0;
                col = 7;
            }
        }
        if (buttonPressed(RIGHTB)) {
            if (col != 7) { 
                ++col;
            } else if (row) {
                --row;
                col = 0;
            } else {
                row = 7;
                col = 0;
            }
        }
        if (buttonPressed(BACKB)) {
            --row;
            if (row < 0) row += 8;
        }

        // toggle cell at cursor
        if (buttonPressed(SELECTB) && game.table[row][col] && (game.table[row][col] >> 1 & 1) == game.player) {
            selected = true;
        }
    }

    printMatrix();
    
    delay(5);
}

// TODO prints rotated 90 degrees counterclockwise
void printMatrix()
{
    unsigned char b;
    int i, j, diff;

    curMillis = millis();
    diff = curMillis - preMillis;

    // TODO not concurrent, PL2 doesn't blink
    // update preMillis every 200 milliseconds, but measure diff at 100 and 150 milliseconds
    if (diff >= PERIOD) {
        preMillis = curMillis;
        pl2Blinking = cursorBlinking = false;
    } else if (diff >= PL2BLINK) {
        pl2Blinking = true;
    } else if (diff >= CURSORBLINK) {
        cursorBlinking = true;
    }

    for (j = 0; j <= 7; ++j) {
        b = 0;
        for (i = 0; i <= 7; ++i) {
            b <<= 1;
            b ^= game.table[i][j] & 1;
            if (pl2Blinking && (game.table[i][j] & 2)) // blink player 2
                b ^= 1;
            if (cursorBlinking && !automatic && i == row && j == col) // blink cursor
                b ^= 1;
        }
        Write_Max7219(1 + j, b);
    }
}

bool buttonPressed(int button)
{
    if (button < LEFTB || button > SELECTB)
        return false;
    int *b = buttons[button - 2];
    b[0] = digitalRead(button);
    if (b[0] && !b[1]) {
        b[1] = b[0];
        return true;
    }
    b[1] = b[0];
    return false;
}

/* 
 * LED Matrix functions 
 * Taken from https://s3.amazonaws.com/linksprite/Arduino_kits/LEDmatrixkit/LED_Martix_Kit_User_Guide_V2.pdf
 */
 
void Init_MAX7219()
{
  Write_Max7219(0x09, 0x00);       //decoding ：BCD
  Write_Max7219(0x0a, 0x00);       //brightness 
  Write_Max7219(0x0b, 0x07);       //scanlimit；8 LEDs
  Write_Max7219(0x0c, 0x01);       //power-down mode：0，normal mode：1
  Write_Max7219(0x0f, 0x00);       //test display：1；EOT，display：0
}
 
void Write_Max7219(unsigned char address, unsigned char dat)
{
  digitalWrite(Max7219_pinCS, LOW);
  Write_Max7219_byte(address);           //address，code of LED
  Write_Max7219_byte(dat);               //data，figure on LED 
  digitalWrite(Max7219_pinCS, HIGH);
}

void Write_Max7219_byte(unsigned char data) 
{   
  unsigned char i;
  digitalWrite(Max7219_pinCS, LOW);  
        
  for (i = 8; i >= 1; --i) {       
     digitalWrite(Max7219_pinCLK, LOW);
     digitalWrite(Max7219_pinDIN, data & 0x80);// Extracting a bit data
     data <<= 1;
     digitalWrite(Max7219_pinCLK, HIGH);
  }                                 
}
