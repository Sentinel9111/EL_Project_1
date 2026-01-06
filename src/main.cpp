#include <Arduino.h>
#include "headers.h"
// 16 LDR's "4x4" grid

// [0 ] [1 ] [2 ] [3 ]

// [4 ] [5 ] [6 ] [7 ]

// [8 ] [9 ] [10] [11]

// [12] [13] [14] [15]

// Randomise mines (DONE)
// Detect player with LDR
// Trigger mine when player stands on it
// Mine deactivates after trigger
// Timer and dice
// Multiplexer

#define BOARD_SIZE 16
#define MINE_COUNT 6

bool hasMine[BOARD_SIZE];

#define LDR_PIN 34 // test ldr
#define LDR_THRESHOLD 100 // adjust based on light

bool pawnPresent() {
    int value = analogRead(LDR_PIN);
    return value < LDR_THRESHOLD; // returns true if pawn is present
}

void setup() {
    Serial.begin(9600);
    delay(5000); // Time to open serial monitor

    randomSeed(esp_random());
    setupMines();
}

void loop() {
    checkMines();
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
}


bool previousState = false;
bool mineTriggered = false;
void checkMines() {
    if (mineTriggered) return; // only trigger a mine once

    bool currentState = pawnPresent(); // only trigger a mine when a pawn arrives

    if (!previousState && currentState) {
        mineTriggered = true;
        Serial.println("Mine triggered");
    }
    previousState = currentState;
}