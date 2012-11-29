#include <LPD8806.h>
#include <SPI.h>

#define LED_COUNT (42)

const byte greenFrequency = 19;
const byte redFrequency = 13;
const byte blueFrequency = 9;

const byte intervalGreen = 11;
const byte intervalRed = 7;
const byte intervalBlue = 5;

byte waveStep = 0;
long currentMillis = 0;
long nextGreen = 0;
long nextRed = 0;
long nextBlue = 0;
long currentColor = 0;

LPD8806 strip = LPD8806(LED_COUNT);

void setup() {
  // Start up the LED strip
  strip.begin();
  strip.show();
}

void loop() {
  currentMillis = millis();
  greenWave();
  redWave();
  blueWave();
  strip.show();
}

void redWave() {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextRed) {
    nextRed += intervalRed;
    for (byte index = 0; index < LED_COUNT; index++) {
      switch ((index + pos) % redFrequency) {
        case 0:
          setPixelRed(index, counter / 2);
          break;
        case 1:
          setPixelRed(index, 15 + counter);
          break;
        case 2:
          setPixelRed(index, 45 + counter * 2);
          break;
        case 3:
          setPixelRed(index, 105 - counter * 2);
          break;
        case 4:
          setPixelRed(index, 45 - counter);
          break;
        case 5:
          setPixelRed(index, 15 - counter / 2);
          break;
        default:
          setPixelRed(index, 0);
      }
    }
    counter = ++counter % 30;
    if (counter == 0) {
      pos++; // run red lights in forward direction
    }
  }
}

void blueWave() {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextBlue) {
    nextBlue += intervalBlue;
    for (byte index = 0; index < LED_COUNT; index++) {
      switch ((index + pos) % blueFrequency) {
        case 0:
          setPixelBlue(index, counter / 2);
          break;
        case 1:
          setPixelBlue(index, 15 + counter);
          break;
        case 2:
          setPixelBlue(index, 45 + counter * 2);
          break;
        case 3:
          setPixelBlue(index, 105 - counter * 2);
          break;
        case 4:
          setPixelBlue(index, 45 - counter);
          break;
        case 5:
          setPixelBlue(index, 15 - counter / 2);
          break;
        default:
          setPixelBlue(index, 0);
      }
    }
    counter = ++counter % 30;
    if (counter == 0) {
      pos++; // run blue lights in forward direction
    }
  }
}

void greenWave() {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextGreen) {
    nextGreen += intervalGreen;
    for (byte index = 0; index < LED_COUNT; index++) {
      switch ((index + pos) % greenFrequency) {
        case 0:
          setPixelGreen(index, 15 - counter / 2);
          break;
        case 1:
          setPixelGreen(index, 45 - counter);
          break;
        case 2:
          setPixelGreen(index, 105 - counter * 2);
          break;
        case 3:
          setPixelGreen(index, 45 + counter * 2);
          break;
        case 4:
          setPixelGreen(index, 15 + counter);
          break;
        case 5:
          setPixelGreen(index, counter / 2);
          break;
        default:
          setPixelGreen(index, 0);
      }
    }
    counter = ++counter % 30;
    if (counter == 0) {
      pos--; // run green lights in reverse direction
    }
  }
}

void setPixelBlue(byte index, byte blue) {
  currentColor = strip.getPixelColor(index) & 0xFFFF00;
  strip.setPixelColor(index, currentColor | blue);
}

void setPixelGreen(byte index, long green) {
  currentColor = strip.getPixelColor(index) & 0x00FFFF;
  strip.setPixelColor(index, currentColor | (green << 16));
}

void setPixelRed(byte index, byte red) {
  currentColor = strip.getPixelColor(index) & 0xFF00FF;
  strip.setPixelColor(index, currentColor | (red << 8));
}
