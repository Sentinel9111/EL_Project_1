#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "headers.h"

// ST7735S TFT screen
#define TFT_CS   5
#define TFT_DC   4
#define TFT_RST  2
#define TFT_SCLK 18
#define TFT_MOSI 23

// multiplexer
#define MUX_SIG 34 // ADC input
#define MUX_S0 16
#define MUX_S1 17
#define MUX_S2 25
#define MUX_S3 26

// buttons
#define BUTTON 32
#define RESET_BUTTON 33

#define BOARD_SIZE 16 // amount of LDRs
#define MINE_COUNT 6 // amount of mines
#define PLAYER_TIME 120 // amount of time in seconds each player has
#define LDR_THRESHOLD 100 // adjust based on light

static_assert(MINE_COUNT <= BOARD_SIZE, "More mines than LDRs!"); // assert if MINE_COUNT is allowed
// kijk Arnold ik heb vrijwillig een assertion geschreven!

// Display layout
#define TIMER_X  10
#define P1_Y     20
#define P2_Y     90
#define DIE_X   130
#define DIE_Y   50

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

struct Mine {
    int index; // LDR mux channel
    bool triggered; // has mine exploded?
    bool previousState;
};

Mine mines[MINE_COUNT];
bool hasMine[BOARD_SIZE];

enum State {
    IDLE, // waiting for game to start
    RUNNING, // game is running
    GAME_OVER // all mines exploded or time ran out
};

State state = IDLE;

int playerTime[2] = {PLAYER_TIME, PLAYER_TIME};
int currentPlayer = 0;
int dieValue = 0;

// millis()
unsigned long lastScan = 0;
constexpr unsigned long scanInterval = 50;
unsigned long lastSecondTick = 0;
static unsigned long lastDisplayUpdate = 0;

// button
volatile bool buttonPressed = false;
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 100;
// reset button
volatile bool resetPressed = false;
unsigned long lastResetPress = 0;

// button interrupt
void IRAM_ATTR buttonInterrupt() {
    unsigned long now = millis();
    if (now - lastButtonPress > debounceDelay) {
        buttonPressed = true;
        lastButtonPress = now;
    }
}

// reset button interrupt
void IRAM_ATTR resetInterrupt() {
    unsigned long now = millis();
    if (now - lastResetPress > debounceDelay) {
        resetPressed = true;
        lastResetPress = now;
    }
}

void setup() {
    Serial.begin(9600);
    SPI.begin(TFT_SCLK, -1, TFT_MOSI);
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    pinMode(MUX_S0, OUTPUT);
    pinMode(MUX_S1, OUTPUT);
    pinMode(MUX_S2, OUTPUT);
    pinMode(MUX_S3, OUTPUT);

    pinMode(BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON), buttonInterrupt, FALLING);
    pinMode (RESET_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RESET_BUTTON), resetInterrupt, FALLING);

    randomSeed(esp_random());
    setupMines();

    lastSecondTick = millis();
}

void loop() {
    // poll mines
    if (millis() - lastScan >= scanInterval) {
        lastScan = millis();
        checkMines();
    }

    // update timer and display
    updateTimers();
    updateDisplay();

    // handle button press
    if (buttonPressed) {
        buttonPressed = false;
        Serial.println("button pressed");
        buttonPress();
    }
    // handle reset button press
    if (resetPressed) {
        resetPressed = false;
        Serial.println("reset pressed");
        resetGame();
    }
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

    // place mines randomly
    int placedMines = 0;
    while (placedMines < MINE_COUNT) {
        int pos = random(0, BOARD_SIZE);
        if (!hasMine[pos]) {
            hasMine[pos] = true;
            placedMines++;
        }
    }

    // print mines to serial
    Serial.print("Mines at positions: ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (hasMine[i]) {
            Serial.print(i);
            Serial.print(" ");
        }
    }
    Serial.println();

    // print mines in grid to serial
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
    if (state != RUNNING) return;

    for (int i = 0; i < MINE_COUNT; i++) {
        if (mines[i].triggered) continue; // skip triggered mines

        bool currentState = pawnPresentAt(mines[i].index);

        if (!mines[i].previousState && currentState) { // only trigger a mine when a pawn arrives
            mines[i].triggered = true; // mine can only explode once
            Serial.print("Mine triggered: ");
            Serial.println(i);

            if (allMinesTriggered()) {
                state = GAME_OVER;
                Serial.println("All mines have been exploded");
            }
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

// player timers
void updateTimers() {
    if (state != RUNNING) return;

    if (millis() - lastSecondTick >= 1000) {
        lastSecondTick += 1000;
        if (playerTime[currentPlayer] > 0) {
            playerTime[currentPlayer]--;
        }

        // check if the player ran out of time
        if (playerTime[currentPlayer] <= 0) {
            state = GAME_OVER;
            Serial.print("Player #");
            Serial.print(currentPlayer + 1);
            Serial.println(" ran out of time");
        }
    }
}

// roll a d4 die
void rollDie() {
    dieValue = random(1, 5);
    Serial.print("Die: ");
    Serial.print(dieValue);
    Serial.println();
}

// roll die and switch player when button is pressed
void buttonPress() {
    rollDie();

    if (state == IDLE) {
        state = RUNNING;
        lastSecondTick = millis();
    } else if (state == RUNNING) {
        currentPlayer = 1 - currentPlayer;
        Serial.print("Player #");
        Serial.print(currentPlayer);
        Serial.println("'s turn");
    }
}

// reset the game if the reset button is pressed
void resetGame() {
    state = IDLE;
    currentPlayer = 0;
    playerTime[0] = PLAYER_TIME;
    playerTime[1] = PLAYER_TIME;
    setupMines();

    lastSecondTick = millis();
    tft.fillScreen(ST77XX_BLACK);
}

// check if all mines have been exploded
bool allMinesTriggered() {
    for (int i = 0; i < MINE_COUNT; i++) {
        if (!mines[i].triggered) return false;
    }
    return true;
}

// draws the timers
void drawPlayerTimer(int seconds, int y, bool active) {
    int min = seconds / 60;
    int sec = seconds % 60;

    char buf[6];
    sprintf(buf, "%02d:%02d", min, sec);

    tft.setTextSize(3);
    tft.setTextColor(active ? ST77XX_GREEN : ST77XX_WHITE, ST77XX_BLACK);
    tft.setCursor(TIMER_X, y);
    tft.print(buf);
}

// draws the die
void drawDie(int value) {
    tft.setTextSize(4);
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.setCursor(DIE_X, DIE_Y);
    tft.print(value);
}

// updates the display
void updateDisplay() {
    if (millis() - lastDisplayUpdate < 200) return;
    lastDisplayUpdate = millis();

    drawPlayerTimer(playerTime[0], P1_Y, state == RUNNING && currentPlayer == 0);
    drawPlayerTimer(playerTime[1], P2_Y, state == RUNNING && currentPlayer == 1);

    if (state == GAME_OVER) {
        tft.setTextSize(2);
        tft.setCursor(10, 60);
        tft.setTextColor(ST77XX_RED);
        tft.print("Game Over");
    } else {
        drawDie(dieValue);
    }
}