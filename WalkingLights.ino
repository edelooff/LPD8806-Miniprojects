#include <LPD8806.h>
#include <SPI.h>

#define LED_COUNT (42)

const byte greenFrequency = 19;
const byte redFrequency = 13;
const byte blueFrequency = 9;

const byte intervalGreen = 3;
const byte intervalRed = 7;
const byte intervalBlue = 5;

long currentMillis = 0;
long nextGreen = 0;
long nextRed = 0;
long nextBlue = 0;

boolean enableGreen = true;
boolean enableRed = true;
boolean enableBlue = true;

LPD8806 strip = LPD8806(LED_COUNT);

void setup() {
  // Start up the LED strip
  Serial.begin(9600);
  strip.begin();
  strip.show();
}

void loop() {
  currentMillis = millis();
  if (Serial.available()) {
    switch (Serial.read()) {
      case 'r':
        enableRed = !enableRed;
        break;
      case 'g':
        enableGreen = !enableGreen;
        break;
      case 'b':
        enableBlue = !enableBlue;
        break;
    }
  }
  (enableGreen) ? greenWave() : noGreenWave();
  (enableRed)   ? redWave()   : noRedWave();
  (enableBlue)  ? blueWave()  : noBlueWave();
  strip.show();
}

void noGreenWave() {
  for (byte index = LED_COUNT; index-- > 0;) {
    setPixelGreen(index, 0);
  }
}

void noRedWave() {
  for (byte index = LED_COUNT; index-- > 0;) {
    setPixelRed(index, 0);
  }
}

void noBlueWave() {
  for (byte index = LED_COUNT; index-- > 0;) {
    setPixelBlue(index, 0);
  }
}

void redWave() {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextRed) {
    nextRed = currentMillis + intervalRed;
    for (byte index = LED_COUNT; index-- > 0;) {
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
      pos = ++pos % redFrequency; // pos++; // run red lights in forward direction
    }
  }
}

void blueWave() {
  byte index;
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextBlue) {
    nextBlue = currentMillis + intervalBlue;
    for (index = LED_COUNT; index-- > 0;) {
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
      pos = ++pos % blueFrequency; // run blue lights in forward direction
    }
  }
}

void greenWave() {
  byte index;
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextGreen) {
    nextGreen = currentMillis + intervalGreen;
    for (index = LED_COUNT; index-- > 0;) {
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
      if (pos == 0)
        pos = greenFrequency - 1;
      else
        pos = --pos % (greenFrequency); // run green lights in reverse direction
    }
  }
}

void setPixelBlue(byte index, byte blue) {
  strip.setPixelColor(index, strip.getPixelColor(index) & 0xFFFF00 | blue);
}

void setPixelGreen(byte index, long green) {
  strip.setPixelColor(index, strip.getPixelColor(index) & 0x00FFFF | (green << 16));
}

void setPixelRed(byte index, byte red) {
  strip.setPixelColor(index, strip.getPixelColor(index) & 0xFF00FF | (red << 8));
}
