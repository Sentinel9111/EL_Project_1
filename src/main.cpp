#include <Arduino.h>
#include "headers.h"
// 16 LDR's "4x4" grid

// [0 ] [1 ] [2 ] [3 ]

// [4 ] [5 ] [6 ] [7 ]

// [8 ] [9 ] [10] [11]

// [12] [13] [14] [15]

// Random mines
//

#define BOARD_SIZE 16
#define MINE_COUNT 6

bool hasMine[BOARD_SIZE];

void setup() {
// write your initialization code here
    Serial.begin(9600);
    delay(5000); // Time to open serial monitor

    randomSeed(esp_random());
    setupMines();
}

void loop() {
// write your code here

}

void setupMines() {
    // Clear board
    for (int i = 0; i < BOARD_SIZE; i++) {
        hasMine[i] = false;
    }

    int placedMines = 0;
    while (placedMines < MINE_COUNT) {
        int pos = random(0, BOARD_SIZE);
        if (!hasMine[pos]) {
            hasMine[pos] = true;
            placedMines++;
        }
    }

    // Debug output
    Serial.print("Mines at positions: ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (hasMine[i]) {
            Serial.print(i);
            Serial.print(" ");
        }
    }
    Serial.println();

    Serial.println("Mine grid:");
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int index = row * 4 + col;
            Serial.print(hasMine[index] ? "X " : ". ");
        }
        Serial.println();
    }
}