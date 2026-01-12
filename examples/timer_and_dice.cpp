#include <Arduino.h>
#include <Ticker.h>
#include <TM1637Display.h>

#define CLK 22
#define DIO 21
TM1637Display display(CLK, DIO);

const int knop = 19;
bool startSpel = false;

const unsigned long debounceTime = 200; // ms
unsigned long lastButtonTime = 0;

bool lastButtonState = HIGH;

bool gameIsOngoing = false;

int klokSpeler1 = 240;
int klokSpeler2 = 240;
int wieIsAanDeBeurt = 1;

int huidigeTijd = 0;

int diceDisplayTimer = 0;
int diceValue = 0;

Ticker ticker;

int currentState = 0;

int rollDice() {
  return random(1, 5); // 1 to 4
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
    diceDisplayTimer = diceDisplayTimer - 1;
    HandleDisplay();

  }
}

void setup() {

  pinMode(knop, INPUT_PULLUP);
  Serial.begin(9600);
  ticker.attach(1.0, tick);
  display.setBrightness(0x0f);
  randomSeed(esp_random());

}

void loop() {



  //knop
  bool buttonState = digitalRead(knop);

  if (buttonState == LOW && lastButtonState == HIGH) { //knop indrukken met debouncetime
    if (millis() - lastButtonTime > debounceTime) {
      KnopFunctie();
      lastButtonTime = millis();
    }
  }

  lastButtonState = buttonState;

  switch(currentState) {
    case 0: //niet aan het spelen
      if (startSpel){
        currentState = 1;
      }
      break;

    case 1: // begin spel

      TijdensSpel();
      gameIsOngoing = true;

      break;

  }
}
void TijdensSpel(){ // code tijdens het spel

  switch(wieIsAanDeBeurt){
    case 1:

    break;

    case 2:

    break;
  }
}
void KnopFunctie(){
  if(!gameIsOngoing){
    startSpel = true;
    diceValue = rollDice();
    diceDisplayTimer = 3;
  }
  if(gameIsOngoing){
    diceValue = rollDice();
    diceDisplayTimer = 3;
    if(wieIsAanDeBeurt == 1){
      wieIsAanDeBeurt = 2;
    }
    else{
      wieIsAanDeBeurt = 1;
    }
  }
}
void HandleDisplay(){
  if(diceDisplayTimer <= 0){
    int minuten = huidigeTijd / 60;
    int seconden = huidigeTijd % 60;

    int timeToDisplay = minuten * 100 + seconden;
    display.showNumberDecEx(timeToDisplay, 0b01000000, true);
  }
  else{
    display.showNumberDecEx(diceValue, 0b01000000, true);
  }

}