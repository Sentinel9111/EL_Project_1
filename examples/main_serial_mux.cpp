#include <Arduino.h>
#include "headers.h"

#define MUX_SIG 34 // ADC input
#define MUX_S0 16
#define MUX_S1 17
#define MUX_S2 25
#define MUX_S3 26

#define BOARD_SIZE 16 // amount of LDRs
#define MINE_COUNT 6 // amount of mines
#define LDR_THRESHOLD 100 // adjust based on light

static_assert(MINE_COUNT <= BOARD_SIZE, "More mines than LDRs!"); // assert if MINE_COUNT is allowed

bool hasMine[BOARD_SIZE];

struct Mine {
    int index; // LDR mux channel
    bool triggered; // has mine exploded?
    bool previousState;
};

Mine mines[MINE_COUNT];

void setup() {
    Serial.begin(9600);
    delay(5000); // time to open serial monitor

    pinMode(MUX_S0, OUTPUT);
    pinMode(MUX_S1, OUTPUT);
    pinMode(MUX_S2, OUTPUT);
    pinMode(MUX_S3, OUTPUT);

    randomSeed(esp_random());
    setupMines();
}

void loop() {
    checkMines();
    delay(50); // polling, could replace with millis
}

// randomly place mines on the board and initialize mines
void setupMines() {
    // clear board
    for (int i = 0; i < BOARD_SIZE; i++) {
        hasMine[i] = false;
    }

    // clear mine index
    for (int i = 0; i < MINE_COUNT; i++) {
        mines[i].index = -1;
        mines[i].triggered = true;
        mines[i].previousState = false;
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

    // loop over every square and index mines
    int mineIndex = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (hasMine[i]) {
            mines[mineIndex].index = i;
            mines[mineIndex].triggered = false;
            mines[mineIndex].previousState = false;
            mineIndex++;
        }
    }
}

// poll all active mines and detect when pawn arrives at an LDR square
void checkMines() {
    for (int i = 0; i < MINE_COUNT; i++) {
        if (mines[i].triggered) continue; // skip triggered mines

        bool currentState = pawnPresentAt(mines[i].index);

        if (!mines[i].previousState && currentState) { // only trigger a mine when a pawn arrives
            mines[i].triggered = true; // mine can only explode once
            Serial.print("Mine triggered: ");
            Serial.println(i);
        }
        mines[i].previousState = currentState;
    }
}

// select multiplexer channel
void muxSelect(int channel) {
    digitalWrite(MUX_S0, channel & 0x01);
    digitalWrite(MUX_S1, channel & 0x02);
    digitalWrite(MUX_S2, channel & 0x04);
    digitalWrite(MUX_S3, channel & 0x08);
}

// check if pawn is present on an LDR square
bool pawnPresentAt(int index) {
    muxSelect(index);
    delayMicroseconds(5); // wait for mux

    // account for noise at the cost of 5 microseconds
    int value1 = analogRead(MUX_SIG);
    delayMicroseconds(5);
    int value2 = analogRead(MUX_SIG);

    int value = (value1 + value2) / 2;
    return value < LDR_THRESHOLD;
}