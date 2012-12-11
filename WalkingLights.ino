#include <SPI.h>

#define LED_COUNT (158)
#define RED_OFFSET (1)
#define GREEN_OFFSET (2)
#define BLUE_OFFSET (0)

const byte redFrequency = 12;
const byte greenFrequency = 15;
const byte blueFrequency = 19;

const byte intervalRed = 10;
const byte intervalGreen = 12;
const byte intervalBlue = 15;

byte intensityRed;
byte intensityGreen;
byte intensityBlue;

long currentMillis = 0;
long nextRed = 0;
long nextGreen = 0;
long nextBlue = 0;

boolean enableRed = true;
boolean enableGreen = true;
boolean enableBlue = true;

byte index;
byte *pixels;

void setup() {
  // Set up the LED-strip
  pixels = (byte*)malloc(LED_COUNT * 3 + 1);
  memset(pixels, 0x80, LED_COUNT * 3);
  pixels[LED_COUNT * 3] = 0;

  // Start SPI communication for the LEDstrip
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPDR = 0; // 'Prime' the SPI bus with initial latch (no wait)

  Serial.begin(9600);
}

void loop() {
  currentMillis = millis();
  if (Serial.available()) {
    switch (Serial.read()) {
      case 'r':
        if (enableRed)
          noRedWave();
        enableRed = !enableRed;
        break;
      case 'g':
        if (enableGreen)
          noGreenWave();
        enableGreen = !enableGreen;
        break;
      case 'b':
        if (enableBlue)
          noBlueWave();
        enableBlue = !enableBlue;
        break;
    }
  }
  if (enableGreen) greenWave();
  if (enableRed)   redWave();
  if (enableBlue)  blueWave();
  show();
}

void show(void) {
  unsigned int i, n3 = LED_COUNT * 3 + 1;
  for (i=0; i<n3; i++ ) {
    while(!(SPSR & (1<<SPIF))); // Wait for prior byte out
    SPDR = pixels[i];           // Issue new byte
  }
}

void noGreenWave() {
  for (index = LED_COUNT; index --> 0;) {
    pixels[index * 3 + GREEN_OFFSET] = 0x80;
  }
}

void noRedWave() {
  for (index = LED_COUNT; index --> 0;) {
    pixels[index * 3 + RED_OFFSET] = 0x80;
  }
}

void noBlueWave() {
  for (index = LED_COUNT; index --> 0;) {
    pixels[index * 3 + BLUE_OFFSET] = 0x80;
  }
}

void redWave() {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextRed) {
    nextRed = currentMillis + intervalRed;
    for (index = 0; index < LED_COUNT; ++index) {
      switch ((index + pos) % redFrequency) {
        case 0:
          intensityRed = counter / 2;
          break;
        case 1:
          intensityRed = 15 + counter;
          break;
        case 2:
          intensityRed = 45 + counter * 2;
          break;
        case 3:
          intensityRed = 105 - counter * 2;
          break;
        case 4:
          intensityRed = 45 - counter;
          break;
        case 5:
          intensityRed = 15 - counter / 2;
          break;
        default:
          intensityRed = 0;
      }
      pixels[index * 3 + RED_OFFSET] = 128 | intensityRed;
    }
    counter = (counter + 3) % 30;
    if (counter == 0) {
      pos = ++pos % redFrequency; // pos++; // run red lights in forward direction
    }
  }
}

void blueWave() {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextBlue) {
    nextBlue = currentMillis + intervalBlue;
    for (index = 0; index < LED_COUNT; ++index) {
      switch ((index + pos) % blueFrequency) {
        case 0:
          intensityBlue = counter / 2;
          break;
        case 1:
          intensityBlue = 15 + counter;
          break;
        case 2:
          intensityBlue = 45 + counter * 2;
          break;
        case 3:
          intensityBlue = 105 - counter * 2;
          break;
        case 4:
          intensityBlue = 45 - counter;
          break;
        case 5:
          intensityBlue = 15 - counter / 2;
          break;
        default:
          intensityBlue = 0;
      }
      pixels[index * 3 + BLUE_OFFSET] = 128 | intensityBlue;
    }
    counter = (counter + 3) % 30;
    if (counter == 0) {
      pos = ++pos % blueFrequency; // run blue lights in forward direction
    }
  }
}

void greenWave() {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextGreen) {
    nextGreen = currentMillis + intervalGreen;
    for (index = 0; index < LED_COUNT; ++index) {
      switch ((index + pos) % greenFrequency) {
        case 0:
          intensityGreen = 15 - counter / 2;
          break;
        case 1:
          intensityGreen = 45 - counter;
          break;
        case 2:
          intensityGreen = 105 - counter * 2;
          break;
        case 3:
          intensityGreen = 45 + counter * 2;
          break;
        case 4:
          intensityGreen = 15 + counter;
          break;
        case 5:
          intensityGreen = counter / 2;
          break;
        default:
          intensityGreen = 0;
      }
      pixels[index * 3 + GREEN_OFFSET] = 128 | intensityGreen;
    }
    counter = (counter + 3) % 30;
    if (counter == 0) {
      if (pos == 0)
        pos = greenFrequency - 1;
      else
        --pos; // run green lights in opposite direction
    }
  }
}
