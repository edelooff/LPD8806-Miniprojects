#include <SPI.h>

#define LED_COUNT (158)
const byte
  // The following two constants define the background behaviour.
  // The number of background levels is just that, the number of final
  // brightness levels that can be set as target
  brightnessLevels = 4,
  // The step size determines the position between the brightness levels.
  // 4 and 4 result in the following brightness intensities; 4, 8, 12, and 16.
  // Each update interval the current brightness is adjusted towards the target
  // brightness by 1. After a number of steps equal to the step size, new random
  // targets are chosen.
  brightnessStepSize = 4,
  blinkBrightness[] = {0, 1, 2, 3, 5, 8, 13, 21, 32, 46, 64, 88, 116,
                       88, 64, 46, 32, 21, 13, 8, 5, 3, 2, 1},
  blinkBrightnessSteps = sizeof(blinkBrightness),
  highlightCount = 1,
  // Interval times (in milliseconds) between updates and starts of highlights.
  intervalUpdate = 30,
  intervalHighlight = 100;
byte
  // All brightness output information
  stripBackgroundCurrent[LED_COUNT],
  stripBackgroundTarget[LED_COUNT],
  stripHighlights[LED_COUNT];
long
  // Timekeeping variables
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
  static byte counter = 0;
  currentMillis = millis();
  if (currentMillis >= nextHighlight) {
    nextHighlight += intervalHighlight;
    addHighlights(highlightCount);
  }
  if (currentMillis >= nextUpdate) {
    nextUpdate += intervalUpdate;
    if (!counter)
      newBackground();
    updateBackground();
    updateHighlights();
    show();
    counter = ++counter % brightnessStepSize;
  }
}

void show(void) {
  int colorChannels, brightness;
  SPI.transfer(0);
  for (byte index = 0; index < LED_COUNT; ++index) {
    // The brightness of any given LED is the sum of its background and highlight
    // brightness. The highlight brightness is retrieved from the blinkBrightnessLevels
    // array which has a plain logarithmic envelope (causing human eyes to see more or
    // less linear increase and decrease in brightness).
    brightness = min(127, (stripBackgroundCurrent[index] +
                           blinkBrightness[stripHighlights[index]]));
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
    if (!stripHighlights[index]) {
      stripHighlights[index] = 1;
    }
  }
}

void newBackground(void) {
  // Set new background targets to slowly progress to.
  byte *p = &stripBackgroundTarget[0];
  for (int index = LED_COUNT; index-- > 0;) {
    *p++ = (random(brightnessLevels) + 1) * brightnessStepSize;
  }
}

void updateBackground(void) {
  // Changes the background color slightly, ensuring there is some pleasing variety.
  for (int index = LED_COUNT; index-- > 0;) {
    if (stripBackgroundTarget[index] > stripBackgroundCurrent[index]) {
      ++stripBackgroundCurrent[index];
    } else if (stripBackgroundTarget[index] < stripBackgroundCurrent[index]) {
      // We need this second comparison so that the current background color
      // is NOT changed when the current is equal to the target.
      // Changing it regardless causes highly unpleasant flickering.
      --stripBackgroundCurrent[index];
    }
  }
}

void updateHighlights(void) {
  // Increments the brightness index for all blinking highlights.
  for (int index = LED_COUNT; index-- > 0;) {
    if (stripHighlights[index]) {
      stripHighlights[index] = ++stripHighlights[index] % blinkBrightnessSteps;
    }
  }
}
