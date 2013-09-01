/*
 * RunningColorDot by Elmer de Looff from Frack - Hackerspace Friesland
 *
 * This is the Arduino (C++) source code for the project demonstrated here.
 * It paints a color-changing dot that quickly travels around the strip, leaving
 * a short trail in its wake. On every step, the dot has a small chance of
 * reversing direction, which is made more obvious by showing a 'block' on the
 * strip. Once placed, these reminders serve no purpose and are 'eaten up' by
 * the light trail whenever it encounters them again
 *
 * This and more available on https://github.com/edelooff/LPD8806-Miniprojects
 * Interested in what we do at Frack? check out our wiki: http://frack.nl/wiki
 */

#include <LPD8806.h> // Obtained from https://github.com/adafruit/LPD8806
#include <SPI.h>     // Provides SPI communication
#include "color.h"   // Defines struct rgbdata_t { byte r, g, b; };

// Function declarations for command line compilation
long attenuateColor(rgbdata_t, byte);
void moveTrail();
void paintBlock(int, char, rgbdata_t);
void paintTrail(rgbdata_t);
rgbdata_t rotatingWheelColor();

const int
  collisionPauseTime = 100,
  colorChangeInterval = 2,
  colorChangesPerStep = 10,
  ledCount = 96,
  trailLength = 6,
  guaranteedSteps = 40,
  swapChanceReciprocal = 100;
LPD8806 strip = LPD8806(ledCount); // Hardware SPI. Pins 11 & 13 on Arduino Uno.
int trail[trailLength];

void setup() {
  strip.begin();
  randomSeed(analogRead(0));
}

void loop() {
  static unsigned int changeCounter = 0;
  static long currentMillis, nextColor = 0;
  currentMillis = millis();
  if (currentMillis >= nextColor) {
    // In-place addition would speed up the animation post-collision
    nextColor = currentMillis + colorChangeInterval;
    if (++changeCounter % colorChangesPerStep == 0)
      moveTrail();
    paintTrail(rotatingWheelColor());
  }
}

void moveTrail(void) {
  static char direction = 1;
  static int stepsWithoutChange = guaranteedSteps;
  strip.setPixelColor(trail[trailLength - 1], 0); // Remove end of current tail.
  for (int index = trailLength; index-- > 1;)
    trail[index] = trail[index - 1];
  if (stepsWithoutChange > 0) {
    --stepsWithoutChange;
  } else if (!random(swapChanceReciprocal)) {
    // Paint block, reverse direction and don't 'collide' for a while
    paintBlock(trail[0], direction, rotatingWheelColor());
    direction *= -1; // toggle direction between forward and backward
    stepsWithoutChange = guaranteedSteps;
  }
  // Add `ledCount` to the head position index so we never modulo negative ints.
  trail[0] = (trail[1] + ledCount + direction) % ledCount;
}

void paintBlock(int index, char direction, rgbdata_t color) {
  index = (index + ledCount + direction) % ledCount;
  strip.setPixelColor(index, attenuateColor(color, 0));
  strip.show();
  delay(collisionPauseTime);
}

void paintTrail(rgbdata_t color) {
  for (int i = trailLength; i-- > 0;)
    strip.setPixelColor(trail[i], attenuateColor(color, i));
  strip.show();
}

long attenuateColor(rgbdata_t color, byte att) {
  return strip.Color(color.r >> att, color.g >> att, color.b >> att);
}

rgbdata_t rotatingWheelColor() {
  static unsigned int wheelPos;
  byte down, up;
  ++wheelPos %= 384; // Iterate through all 384 'degrees', from 0 up to 383.
  down = 127 - wheelPos % 128;
  up = wheelPos % 128;
  switch(wheelPos / 128) {
    case 0:
      return rgbdata_t {down, up, 0};
    case 1:
      return rgbdata_t {0, down, up};
    default:
      return rgbdata_t {up, 0, down};
  }
}
