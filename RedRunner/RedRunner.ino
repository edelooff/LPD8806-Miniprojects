#include <LPD8806.h> // Obtained from https://github.com/adafruit/LPD8806
#include <SPI.h>     // Provides SPI communication
#include "barrier.h" // Defines `barrier_t` struct with `position` and `level`

// Trail and strip variables
const unsigned int
  ledCount = 96,
  stepInterval = 10,
  trailLength = 16,
  trailLevels[trailLength] = {
      127, 106, 88, 72, 58, 46, 36, 27, 20, 14, 10, 6, 4, 2, 1, 0};
unsigned int trail[trailLength] = {};
LPD8806 strip = LPD8806(ledCount); // Hardware SPI. Pins 11 & 13 on Arduino Uno.

// Barrier variables
const unsigned int
  barrierChance = 20,
  barrierInterval = 250,
  barrierLevels[5] = {0, 5, 23, 61, 127},
  barrierStartLevel = 4,
  maxBarriers = 20;
barrier_t barriers[maxBarriers];

void setup() {
  randomSeed(analogRead(0));
  strip.begin();
}

void loop() {
  static long
    nextBarrier = 0,
    nextStep = 0;
  long currentMillis = millis();
  if (currentMillis >= nextStep) {
    nextStep = currentMillis + stepInterval;
    moveTrail();
  }
  if (currentMillis >= nextBarrier) {
    nextBarrier = currentMillis + barrierInterval;
    placeBarrier();
  }
  drawScene();
}

void drawScene() {
  for (int i = trailLength; i-- > 0;)
    strip.setPixelColor(trail[i], trailLevels[i], 0, 0);
  for (int i = maxBarriers; i-- > 0;) {
    barrier_t barrier = barriers[i];
    if (barrier.level)
      strip.setPixelColor(barrier.position, 0, 0, barrierLevels[barrier.level]);
  }
  strip.show();
}

int barrierCollision(unsigned int position) {
  for (int i = maxBarriers; i-- > 0;)
    if (position == barriers[i].position and barriers[i].level)
      return i;
  return -1;
}

int firstNonBarrier() {
  for (int i = maxBarriers; i-- > 0;)
    if (!barriers[i].level)
      return i;
  return -1;
}

void moveTrail(void) {
  static char dir = 1;
  int collision = barrierCollision((trail[1] + ledCount + dir) % ledCount);
  if (collision >= 0)
    if (--barriers[collision].level) // Barrier integrity reduces at collision.
      dir *= -1;                     // Reverse direction if barrier still up.
  for (int i = trailLength; i-- > 1;)
    trail[i] = trail[i - 1];
  // Add `ledCount` to the head position index so we never modulo negative ints.
  trail[0] = (trail[1] + ledCount + dir) % ledCount;
}

void placeBarrier() {
  if (random(100) < barrierChance) {
    int availableIndex = firstNonBarrier();
    if (availableIndex >= 0) {
      int barrierPosition = random(ledCount);
      if (barrierCollision(barrierPosition) < 0 && barrierPosition != trail[0])
        barriers[availableIndex] = {barrierPosition, barrierStartLevel};
    }
  }
}
