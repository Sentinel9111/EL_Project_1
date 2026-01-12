#include <Arduino.h>
#include "headers.h"

#define BOARD_SIZE 16 // amount of LDRs
#define MINE_COUNT 6 // amount of mines
#define LDR_THRESHOLD 100 // adjust based on light

bool hasMine[BOARD_SIZE];

int ldrPins[BOARD_SIZE] = { // LDR pins
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
    // clear board
    for (int i = 0; i < BOARD_SIZE; i++) {
        hasMine[i] = false;
    }
    // place mines
    int placedMines = 0;
    while (placedMines < MINE_COUNT) {
        int pos = random(0, BOARD_SIZE);
        if (!hasMine[pos]) {
            hasMine[pos] = true;
            placedMines++;
        }
    }

    // print mines
    Serial.print("Mines at positions: ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (hasMine[i]) {
            Serial.print(i);
            Serial.print(" ");
        }
    }
    Serial.println();
    // print mines in grid
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
            mines[i].triggered = true; // deactivate mine after exploding
            Serial.print("Mine triggered: ");
            Serial.println(i);
        }
        mines[i].previousState = currentState;
    }
}