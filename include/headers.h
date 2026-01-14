#ifndef EL_PROJECT_1_HEADERS_H
#define EL_PROJECT_1_HEADERS_H

void setupMines();
void checkMines();
void muxSelect(int channel);
bool pawnPresentAt(int index);
void updateTimers();
void rollDie();
void buttonPress();
void resetGame();
bool allMinesTriggered();
void drawPlayerTimer(int seconds, int y, bool active);
void drawDie(int value);
void updateDisplay();
void mineExplode();

#endif //EL_PROJECT_1_HEADERS_H