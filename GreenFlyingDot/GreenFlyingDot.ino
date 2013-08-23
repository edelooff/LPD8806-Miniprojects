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
  // Sends green dot bouncing back and forth over the strip.
  // Each position lights up for the configured duration 'interval'
  static int ledPosition = 0;
  while (ledPosition < ledCount)
    moveAndPause(ledPosition++);
  // ledPosition is post-incremented to ledCount, breaking the loop.
  // We already showed the LED at the end though, so we reduce by 2.
  ledPosition -= 2;
  while (ledPosition > 0)
    moveAndPause(ledPosition--);
}

void moveAndPause(int position) {
  strip.setPixelColor(position, flyingDotColor);
  strip.show();
  strip.setPixelColor(position, backgroundColor);
  delay(interval);
}
