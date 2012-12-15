#include <SPI.h>

#define LED_COUNT (158)
const byte
  backgroundBrightness = 20,
  blinkBrightness[] = {0, 1, 2, 3, 5, 8, 13, 21, 32, 46, 75, 105,
                       75, 46, 32, 21, 13, 8, 5, 3, 2, 1},
  blinkBrightnessLevels = 22,
  highlights = 1,
  intervalUpdate = 25,
  intervalHighlight = 50;
byte
  pixelBackground[LED_COUNT],
  pixelHighlights[LED_COUNT];
long
  currentMillis,
  nextHighlight,
  nextUpdate;

void setup() {
  // Start SPI communication for the LEDstrip
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
//  randomSeed(analogRead(2)); // Don't really need a random seed for this sketch.
}

void loop() {
  currentMillis = millis();
  if (currentMillis >= nextHighlight) {
    nextHighlight += intervalHighlight; 
    addHighlights(highlights);
  }
  if (currentMillis >= nextUpdate) {
    nextUpdate += intervalUpdate;
    updateBackground();
    updateHighlights();
    show();
  }
}

void show(void) {
  byte colorChannels, brightness;
  SPI.transfer(0);
  for (byte index = 0; index < LED_COUNT; ++index) {
    // The brightness of any given LED is the sum of its background and highlight
    // brightness. The highlight brightness is retrieved from the blinkBrightnessLevels
    // array which has a plain logarithmic envelope (causing human eyes to see more or
    // less linear increase and decrease in brightness).
    brightness = (pixelBackground[index] +
                  blinkBrightness[pixelHighlights[index]]);
    // Since we're doing this in monochrome, write three
    // bytes with the brightness value to the strip.
    for (colorChannels = 3; colorChannels-- > 0;) {
      SPI.transfer(0x80 | brightness);
    }
  }
}

void addHighlights(byte number) {
  // Selects a `number` of random LEDs and starts a blink on them.
  // If a blink is already running, it completes as normal and one less
  // blink is started with the next iteration.
  while (number-- > 0) {
    int index = random(LED_COUNT);
    if (!pixelHighlights[index]) {
      pixelHighlights[index] = 1; 
    }
  }
}

void updateBackground(void) {
  // Changes the background color slightly, ensuring there is some pleasing variety.
  for (int index = LED_COUNT; index-- > 0;) {
    if (random(backgroundBrightness) >= pixelBackground[index]) {
      ++pixelBackground[index];
    } else {
      --pixelBackground[index];
    }
  }
}

void updateHighlights(void) {
  // Increments the brightness index for all blinking highlights.
  for (int index = LED_COUNT; index-- > 0;) {
    if (pixelHighlights[index]) {
      pixelHighlights[index] = ++pixelHighlights[index] % blinkBrightnessLevels;
    } 
  }
}
