#ifndef LED_ANIMATIONS_H
#define LED_ANIMATIONS_H

#include <Adafruit_NeoPixel.h>

// -----------------------------
// LED Panel Setup
// -----------------------------
#define LED_COUNT 9 
#define LEFT_LED_PIN 21 //J7 #3
#define RIGHT_LED_PIN 12 //J3
#define FRONT_LED_PIN 13 //J6
#define BACK_LED_PIN 17 //J10

extern Adafruit_NeoPixel leftStrip;
extern Adafruit_NeoPixel rightStrip;
extern Adafruit_NeoPixel frontStrip;
extern Adafruit_NeoPixel backStrip;

extern uint32_t yellowleft;
extern uint32_t yellowright;
extern uint32_t whiteFront;
extern uint32_t redFront;
extern uint32_t redBackDim;
extern uint32_t redBackBright;

void clearStrips();
void handleLEDAnimations();
void updateBackPanel();
void TurnLeft1(Adafruit_NeoPixel &strip);
void TurnRight1(Adafruit_NeoPixel &strip);

#endif
