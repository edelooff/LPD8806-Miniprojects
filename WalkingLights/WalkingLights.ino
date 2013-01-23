#include <SPI.h>

const byte
  redFrequency = 12,
  greenFrequency = 15,
  blueFrequency = 19,
  intervalRed = 16,
  intervalGreen = 12,
  intervalBlue = 8,
  ledCount = 158;
long
  currentMillis,
  nextRed,
  nextGreen,
  nextBlue;
boolean
  enableRed = true,
  enableGreen = true,
  enableBlue = true;

void setup() {
  // Start SPI communication for the LEDstrip
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.transfer(0); // 'Prime' the SPI bus with initial latch (no wait)
  Serial.begin(9600);
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
  // Send three color bytes for all LEDs on the strip.
  for (byte index = ledCount; index-- > 0;) {
    SPI.transfer(0x80 | (enableBlue  ? blueWave(index)  : 0));
    SPI.transfer(0x80 | (enableRed   ? redWave(index)   : 0));
    SPI.transfer(0x80 | (enableGreen ? greenWave(index) : 0));
  }
  // Sending a byte with a MSB of zero enables the output
  // Also resets the index at which new commands start.
  SPI.transfer(0);
}

byte redWave(byte index) {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextRed) {
    nextRed = currentMillis + intervalRed;
    counter = (counter + 3) % 30;
    if (!counter)
      ++pos %= redFrequency; // run red lights in forward direction
  }
  switch ((index + pos) % redFrequency) {
    case 0: return counter / 2;
    case 1: return 15 + counter;
    case 2: return 45 + counter * 2;
    case 3: return 105 - counter * 2;
    case 4: return 45 - counter;
    case 5: return 15 - counter / 2;
    default: return 0;
  }
}

byte blueWave(byte index) {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextBlue) {
    nextBlue = currentMillis + intervalBlue;
    counter = (counter + 3) % 30;
    if (!counter)
      ++pos %= blueFrequency;
  }
  switch ((index + pos) % blueFrequency) {
    case 0: return counter / 2;
    case 1: return 15 + counter;
    case 2: return 45 + counter * 2;
    case 3: return 105 - counter * 2;
    case 4: return 45 - counter;
    case 5: return 15 - counter / 2;
    default: return 0;
  }
}

byte greenWave(byte index) {
  static byte counter = 0;
  static byte pos = 0;
  if (currentMillis >= nextGreen) {
    nextGreen = currentMillis + intervalGreen;
    counter = (counter + 3) % 30;
    if (!counter)
      if (!pos--)
        pos = greenFrequency - 1;
  }
  switch ((index + pos) % greenFrequency) {
    case 0: return 15 - counter / 2;
    case 1: return 45 - counter;
    case 2: return 105 - counter * 2;
    case 3: return 45 + counter * 2;
    case 4: return 15 + counter;
    case 5: return counter / 2;
    default: return 0;
  }
}
