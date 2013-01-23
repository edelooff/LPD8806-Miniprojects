#include <LPD8806.h>
#include <SPI.h>
#include "color.h"

const int
  collisionPauseTime = 100,
  intervalColor = 2,
  intervalStep = 20,
  ledCount = 158,
  trailLength = 6,
  guaranteedSteps = 40,
  swapChanceReciprocal = 100;
LPD8806 strip = LPD8806(ledCount); // Using hardware SPI on pins 11 & 13
int trail[trailLength];

void setup() {
  strip.begin();
  randomSeed(analogRead(0));
}

void loop() {
  // Sends a color-changing dot along the strip
  static long
    nextColor = 0,
    nextStep = 0;
  long currentMillis = millis();
  if (currentMillis >= nextStep) {
    nextStep = currentMillis + intervalStep;
    moveTrail();
  }
  if (currentMillis >= nextColor) {
    nextColor = currentMillis + intervalColor;
    paintTrail(rotatingWheelColor());
  }
}

void moveTrail(void) {
  static boolean forward = true;
  static int changelessSteps = guaranteedSteps;
  strip.setPixelColor(trail[trailLength - 1], 0);
  for (int index = trailLength; index-- > 1;) {
    trail[index] = trail[index - 1];
  }
  if (changelessSteps > 0)
    --changelessSteps;
  if (!random(swapChanceReciprocal) && !changelessSteps) {
    // Change direction, paint a block and set 'safe steps' variable
    paintBlock(trail[0], forward, rotatingWheelColor());
    forward = !forward;
    changelessSteps = guaranteedSteps;
  }
  if (forward) {
    trail[0] = (trail[1] + 1) % ledCount;
  } else {
    trail[0] = (trail[1] + ledCount - 1) % ledCount;
  }
}

void paintBlock(int index, boolean forward, rgbdata_t color) {
  index = (index + ledCount + (forward ? 1 : -1)) % ledCount;
  strip.setPixelColor(index, attenuateColor(color, 0));
  strip.show();
  delay(collisionPauseTime);
}

void paintTrail(rgbdata_t color) {
  for (int i = trailLength; i-- > 0;) {
    strip.setPixelColor(trail[i], attenuateColor(color, i));
  }
  strip.show();
}

long attenuateColor(rgbdata_t color, byte attenuation) {
  return strip.Color(color.r >> attenuation,
                     color.g >> attenuation,
                     color.b >> attenuation);
}

rgbdata_t rotatingWheelColor() {
  byte down, up;
  static unsigned int wheelPos;
  ++wheelPos %= 384; // There are 384 'degrees' on the color wheel
  down = 127 - wheelPos % 128;
  up = wheelPos % 128;
  switch(wheelPos / 128) {
    case 0:
      return rgbdata_t {down, up, 0};
    case 1:
      return rgbdata_t {0, down, up};
    case 2:
      return rgbdata_t {up, 0, down};
  }
}
