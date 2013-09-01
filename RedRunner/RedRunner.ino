#include <LPD8806.h> // Obtained from https://github.com/adafruit/LPD8806
#include <SPI.h>     // Provides SPI communication
#include "barrier.h" // Defines `barrier_t` struct with `position` and `level`

// Trail and strip variables
const unsigned int
  ledCount = 96,
  stepInterval = 10,
  trailLevels[] = {127, 106, 88, 72, 58, 46, 36, 27, 20, 14, 10, 6, 4, 2, 1, 0},
  trailLength = sizeof(trailLevels) / sizeof(int);
int trail[trailLength] = {};
LPD8806 strip = LPD8806(ledCount); // Hardware SPI. Pins 11 & 13 on Arduino Uno.

// Barrier variables
const unsigned int
  barrierChance = 40,
  barrierInterval = 550,
  barrierLevels[] = {6, 22, 56, 127},
  barrierStartLevel = sizeof(barrierLevels) / sizeof(int),
  maxBarriers = 20;
barrier_t barriers[maxBarriers];

// Teleport variables
const unsigned int teleportInterval = 2700;
int teleport[] = {-1, -1};

void setup() {
  randomSeed(analogRead(0));
  strip.begin();
}

void loop() {
  static long
    nextBarrier = 0,
    nextStep = 0,
    nextTeleport = teleportInterval;
  long currentMillis = millis();
  if (currentMillis >= nextStep) {
    nextStep = currentMillis + stepInterval;
    moveTrail();
  }
  if (currentMillis >= nextBarrier) {
    nextBarrier = currentMillis + barrierInterval;
    placeBarrier();
  }
  if (currentMillis >= nextTeleport) {
    nextTeleport = currentMillis + teleportInterval;
    placeTeleport();
  }
  drawScene();
}

void drawScene() {
  for (int i = trailLength; i-- > 0;)
    strip.setPixelColor(trail[i], trailLevels[i], 0, 0);
  for (int i = maxBarriers; i-- > 0;) {
    barrier_t bar = barriers[i];
    if (bar.level)
      strip.setPixelColor(bar.position, 0, 0, barrierLevels[bar.level - 1]);
  }
  if (teleport[0] != -1) {
    strip.setPixelColor(teleport[0], 0, 127, 0);
    strip.setPixelColor(teleport[1], 127, 0, 22);
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
  int bar, teleportLocation;
  teleportLocation = teleportFrom(trail[0]);
  for (int i = trailLength; i-- > 1;)
    trail[i] = trail[i - 1];
  if (teleportLocation >= 0) {
    trail[0] = teleportLocation;
  } else {
    bar = barrierCollision((trail[1] + ledCount + dir) % ledCount);
    if (bar >= 0) {
      if (--barriers[bar].level == 0) // Barrier integrity reduces at collision.
        strip.setPixelColor(barriers[bar].position, 0); // Remove broken barrier.
      dir *= -1;                      // Reverse direction after collision
    }
    trail[0] = (trail[1] + ledCount + dir) % ledCount;
  }
}

unsigned int randomFreeIndex() {
  unsigned int index = random(ledCount);
  while (barrierCollision(index) > 0 ||
         index == trail[0] || index == teleport[0] || index == teleport[1])
    index = random(ledCount);
  return index;
}

void placeBarrier() {
  if (random(100) < barrierChance) {
    int availableIndex = firstNonBarrier();
    if (availableIndex >= 0)
      barriers[availableIndex] = {randomFreeIndex(), barrierStartLevel};
  }
}

void placeTeleport() {
  if (teleport[0] < 0) {
    teleport[0] = randomFreeIndex();
    teleport[1] = randomFreeIndex();
  }
}

int teleportFrom(unsigned int start) {
  int retval = -1;
  if (start == teleport[0]) {
    retval = teleport[1];
    teleport[0] = -1;
    teleport[1] = -1;
  } else if (start == teleport[1]) {
    retval = teleport[0];
    teleport[0] = -1;
    teleport[1] = -1;
  }
  return retval;
}
