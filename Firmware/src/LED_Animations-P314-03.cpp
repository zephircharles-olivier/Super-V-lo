#include <LED_Animations.h>

#define LEFT_SWITCH_PIN 4//gauche vert
#define RIGHT_SWITCH_PIN 5//droite jaune

// -----------------------------
// LED Strip Object Definitions
// -----------------------------
Adafruit_NeoPixel leftStrip(LED_COUNT, LEFT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightStrip(LED_COUNT, RIGHT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel frontStrip(LED_COUNT, FRONT_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel backStrip(LED_COUNT, BACK_LED_PIN, NEO_GRB + NEO_KHZ800);

// -----------------------------
// Color Definitions
// -----------------------------
uint32_t yellowleft = leftStrip.Color(255, 255, 0);
uint32_t yellowright = rightStrip.Color(255, 255, 0);
uint32_t whiteFront = frontStrip.Color(50, 50, 50);
uint32_t redFront = frontStrip.Color(50, 0, 0);
uint32_t redBackDim = backStrip.Color(50, 0, 0);
uint32_t redBackBright = backStrip.Color(150, 0, 0);

int stepIndex = 0;

void handleLEDAnimations()
{
  bool leftOn = digitalRead(LEFT_SWITCH_PIN);
  bool rightOn = digitalRead(RIGHT_SWITCH_PIN);

  if (leftOn)
  {
    TurnLeft1(leftStrip);
    Serial.print("turningLeft");
  }
  else
  {
    leftStrip.clear();
    leftStrip.show();
  }

  if (rightOn)
  {
    TurnRight1(rightStrip);
    Serial.print("turningRight");
  }
  else
  {
    rightStrip.clear();
    rightStrip.show();
  }
}

void clearStrips()
{
  leftStrip.clear();
  rightStrip.clear();
  frontStrip.clear();
  backStrip.clear();
  leftStrip.show();
  rightStrip.show();
  frontStrip.show();
  backStrip.show();
}

void TurnLeft1(Adafruit_NeoPixel &strip)
{
  int indices[] = {5, 4, 3};
  strip.clear();
  strip.setPixelColor(indices[stepIndex], yellowleft);
  strip.show();
  stepIndex = (stepIndex + 1) % 3;
}

void TurnRight1(Adafruit_NeoPixel &strip)
{
  int indices[] = {3, 4, 5};
  strip.clear();
  strip.setPixelColor(indices[stepIndex], yellowright);
  strip.show();
  stepIndex = (stepIndex + 1) % 3;
}
