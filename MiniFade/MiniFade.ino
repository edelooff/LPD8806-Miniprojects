#include <SPI.h>

#define LPD8806_LATCH (0)

const byte stripLength = 160;
const byte fadeDelay = 20; // Time in milliseconds
const byte brightness[] = {1, 3, 7, 15, 31, 63, 127, 63, 31, 15, 7, 3};

void setup() {
  // Start SPI communication
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
}

void loop() {
  for (byte level = 0; level < sizeof(brightness); ++level) {
    for (byte pos = stripLength; pos-- > 0;)
      for (byte repeats = 3; repeats-- > 0;)
        // Write 3x for red, green, and blue values
        SPI.transfer(0x80 | brightness[level]);
    SPI.transfer(LPD8806_LATCH);
    delay(fadeDelay);
  }
}
