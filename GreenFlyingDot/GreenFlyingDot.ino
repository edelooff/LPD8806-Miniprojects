#include <LPD8806.h>
#include <SPI.h>

const unsigned int
  interval = 20,
  ledCount = 158;
LPD8806
  strip = LPD8806(ledCount); // Using hardware SPI on pins 11 & 13
const unsigned long
  backgroundColor = strip.Color(0, 0, 0),
  flyingDotColor = strip.Color(0, 120, 50);

void setup() {
  strip.begin();
}

void loop() {
  // Sends green dot back and forth.
  // Each position lights up for the configured duration
  // Bug: ends of strip light up 2x longer
  static unsigned int ledPosition = 0;
  while (ledPosition < ledCount)
    updateWait(ledPosition++);
  while (ledPosition > 0)
    updateWait(--ledPosition);
}

void updateWait(unsigned int currPosition) {
  static unsigned int lastPosition = 0;
  strip.setPixelColor(lastPosition, backgroundColor);
  strip.setPixelColor(currPosition, flyingDotColor);
  strip.show();
  lastPosition = currPosition;
  delay(interval);
}
