#include <LPD8806.h> // Obtained from https://github.com/adafruit/LPD8806
#include <SPI.h>     // Provides SPI communication

const unsigned int
  stepInterval = 6,
  ledCount = 96,
  trailLength = 16;
unsigned int
  trail[trailLength] = {},
  levels[trailLength] = {127, 106, 88, 72, 58, 46, 36, 27, 20, 14, 10, 6, 4, 2, 1, 0};
LPD8806 strip = LPD8806(ledCount); // Hardware SPI. Pins 11 & 13 on Arduino Uno.

void setup() {
  strip.begin();
}

void loop() {
  long currentMillis;
  static long nextStepTime = 0;
  currentMillis = millis();
  if (currentMillis >= nextStepTime) {
    nextStepTime = currentMillis + stepInterval;
    moveTrail();
    paintTrail();
  }
}

void moveTrail(void) {
  static char dir = 1;
  if (trail[0] + dir == ledCount || trail[0] + dir == -1)
    dir *= -1; // Reverse direction when we hit either end of the strip.
  for (int i = trailLength; i-- > 1;)
    trail[i] = trail[i - 1];
  // Add `ledCount` to the head position index so we never modulo negative ints.
  trail[0] = (trail[1] + ledCount + dir) % ledCount;
}

void paintTrail() {
  for (int i = trailLength; i-- > 0;)
    strip.setPixelColor(trail[i], levels[i], 0, 0);
  strip.show();
}
