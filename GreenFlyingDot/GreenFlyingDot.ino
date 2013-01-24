#include <LPD8806.h>
#include <SPI.h>

const int
  interval = 20,
  ledCount = 158;
LPD8806
  strip = LPD8806(ledCount); // Using hardware SPI on pins 11 & 13
const unsigned long
  backgroundColor = strip.Color(0, 3, 6),
  flyingDotColor = strip.Color(0, 120, 50);

void setup() {
  strip.begin(); // Initialize the strip.
  strip.show();  // Ensure the whole strip is turned off.
}

void loop() {
  // Sends green dot back and forth.
  // Each position lights up for the configured duration
  // Bug: ends of strip light up 2x longer
  static int ledPosition = 0;
  while (ledPosition < ledCount)
    moveAndPause(ledPosition++);
  while (ledPosition > 0)
    moveAndPause(--ledPosition);
}

void moveAndPause(int position) {
  strip.setPixelColor(position, flyingDotColor);
  strip.show();
  strip.setPixelColor(position, backgroundColor);
  delay(interval);
}
