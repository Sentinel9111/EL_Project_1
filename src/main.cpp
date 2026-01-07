#include <Arduino.h>
#include "headers.h"
// 16 LDR's "4x4" grid

// [0 ] [1 ] [2 ] [3 ]

// [4 ] [5 ] [6 ] [7 ]

// [8 ] [9 ] [10] [11]

// [12] [13] [14] [15]

// Randomise mines (DONE)
// Detect player with LDR (DONE)
// Trigger mine when player stands on it (DONE)
// Mine deactivates after trigger (DONE)
// Timer and dice
// Multiplexer

#define BOARD_SIZE 16 // multiplexer nodig voor 16!
#define MINE_COUNT 6 // aantal mijnen
#define LDR_THRESHOLD 100 // adjust based on light

bool hasMine[BOARD_SIZE];

int ldrPins[BOARD_SIZE] = { // ldr pins, mux nodig voor 16!
    15, 2, 4, 22,
    13, 12, 14, 27,
    26, 25, 33, 32,
    35, 34, 39, 36
};

struct Mine {
    int pin;            // LDR pin
    bool triggered;     // has mine exploded?
    bool previousState;
};

Mine mines[MINE_COUNT];

void setup() {
    Serial.begin(9600);
    delay(5000); // time to open serial monitor
    randomSeed(esp_random());

    setupMines();
}

void loop() {
    checkMines();
    delay(50); // polling
}

// set up a new game
void setupMines() {
    // Clear board
    for (int i = 0; i < BOARD_SIZE; i++) {
        hasMine[i] = false;
    }
    // Place mines
    int placedMines = 0;
    while (placedMines < MINE_COUNT) {
        int pos = random(0, BOARD_SIZE);
        if (!hasMine[pos]) {
            hasMine[pos] = true;
            placedMines++;
        }
    }

    // Print mines
    Serial.print("Mines at positions: ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (hasMine[i]) {
            Serial.print(i);
            Serial.print(" ");
        }
    }
    Serial.println();
    // Print mines in grid
    Serial.println("Mine grid:");
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int index = row * 4 + col;
            Serial.print(hasMine[index] ? "X " : ". ");
        }
        Serial.println();
    }
    int mineIndex = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (hasMine[i]) {
            mines[mineIndex].pin = ldrPins[i];
            mines[mineIndex].triggered = false;
            mines[mineIndex].previousState = false;
            mineIndex++;
        }
    }
}

void checkMines() {
    for (int i = 0; i < MINE_COUNT; i++) {
        if (mines[i].triggered) continue; // skip triggered mines

        bool currentState = analogRead(mines[i].pin) < LDR_THRESHOLD;

        if (!mines[i].previousState && currentState) { // only trigger a mine when a pawn arrives
            mines[i].triggered = true; // mine can only explode once
            Serial.print("Mine triggered: ");
            Serial.println(i);
        }
        mines[i].previousState = currentState;
    }
}