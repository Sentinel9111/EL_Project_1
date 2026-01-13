#include <Arduino.h>
#include <Ticker.h>
#include <TM1637Display.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


#define CLK 22
#define DIO 21
TM1637Display display(CLK, DIO);

#define TFT_CS   5
#define TFT_DC   15
#define TFT_RST  21

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//beeldscherm locaties
#define TIMER_X  30
#define P1_Y     20
#define P2_Y     90

#define DICE_X   140
#define DICE_Y   64
#define DICE_W   40
#define DICE_H   50

const int knop = 19;
bool startSpel = false;

const unsigned long debounceTime = 200; // ms
unsigned long lastButtonTime = 0;

bool lastButtonState = LOW;

bool gameIsOngoing = false;

int klokSpeler1 = 240;
int klokSpeler2 = 240;
int wieIsAanDeBeurt = 1;

int huidigeTijd = 0;

int diceDisplayTimer = 0;
int diceValue = 0;

Ticker ticker;

int currentState = 0;

bool buttonPressed = false;

void IRAM_ATTR handleButtonInterrupt() {
  buttonPressed = true;
}


int rollDice() {
  return random(1, 5); // 1 to 4
}

void drawPlayerTimer(int seconds, int y, bool active) {
  int min = seconds / 60;
  int sec = seconds % 60;
  char buf[6];
  sprintf(buf, "%2d:%02d", min, sec);

  uint16_t color = active ? ST77XX_GREEN : ST77XX_WHITE;

  tft.setTextSize(3);
  tft.setCursor(TIMER_X, y);
  tft.setTextColor(color, ST77XX_BLACK);
  tft.print(buf);
}
void drawDice(int value) {
  tft.setTextSize(3);
  tft.setCursor(DICE_X, DICE_Y);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.print(value);
}

void tick(){

  if(gameIsOngoing){
    switch(wieIsAanDeBeurt){
    case 1:
      klokSpeler1 = klokSpeler1 - 1;
      huidigeTijd = klokSpeler1;
    break;

    case 2:
      klokSpeler2 = klokSpeler2 - 1;
      huidigeTijd = klokSpeler2;
    break;
    }


    Serial.print(wieIsAanDeBeurt);

  }
  HandleDisplay();
}


void setup() {

  tft.initR(INITR_BLACKTAB); // ST7735S
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  pinMode(knop, INPUT_PULLUP);
  Serial.begin(9600);
  ticker.attach(1.0, tick);

  randomSeed(esp_random());



  attachInterrupt(digitalPinToInterrupt(knop), handleButtonInterrupt, FALLING);

}

void loop() {



  //knop
  bool buttonState = buttonPressed;

  if (buttonState == LOW && lastButtonState == HIGH) { //knop indrukken met debouncetime
    if (millis() - lastButtonTime > debounceTime) {
      KnopFunctie();
      lastButtonTime = millis();
    }
  }

  lastButtonState = buttonState;
  if (buttonPressed) {
      buttonPressed = false;
    }


}

void KnopFunctie(){
  Serial.print("knop geklikt");
  if(!gameIsOngoing){
    gameIsOngoing = true;
    diceValue = rollDice();

  }
  if(gameIsOngoing){
    diceValue = rollDice();

    if(wieIsAanDeBeurt == 1){
      wieIsAanDeBeurt = 2;
    }
    else{
      wieIsAanDeBeurt = 1;
    }
  }
}
void HandleDisplay(){
  drawPlayerTimer(klokSpeler1, P1_Y, wieIsAanDeBeurt == 1);

  // Player 2 timer
  drawPlayerTimer(klokSpeler2, P2_Y, wieIsAanDeBeurt == 2);

  drawDice(diceValue);
}