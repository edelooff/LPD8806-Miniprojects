#include <LPD8806.h>
#include <SPI.h>
#include "ball.h"

const int
  ledCount = 158,
  ledPerMeter = 52,
  updateInterval = 2;
const float
  gravity = 9.807, // Earth gravity in m/s^2
//  gravity = 1.622, // Lunar gravity in m/s^2
  ledSpacing = 1.0 / ledPerMeter,
  stripCeiling = (ledCount - 1) * ledSpacing,
  timeFactor = updateInterval / 1000.0;
LPD8806
  strip = LPD8806(ledCount);
Ball balls[] = {
    Ball(stripCeiling, 25, .90, 1.2),
    Ball(stripCeiling, 15, .75, 1.2),
    Ball(stripCeiling, 10, .65, 1.0)};
const byte ballCount = sizeof(balls) / sizeof(Ball);

void setup() {
  strip.begin();
  strip.show();
  Serial.begin(57600);
}

void loop() {
  static long nextStep = 0;
  long milliSeconds = millis();
  if (Serial.available())
    serialAddKineticEnergy();
  if (milliSeconds >= nextStep) {
    nextStep += updateInterval;
    for (byte i = ballCount; i-- > 0;)
      balls[i].accelerate(gravity, timeFactor);
    renderDots();
  }
}

void serialAddKineticEnergy(void) {
  byte peekChar = Serial.peek();
  if (peekChar >= '0' && peekChar <= '9') {
    // Adds or subtracts kinetic energy to/from the current ball motion.
    int addedForce = Serial.parseInt();
    for (int i = 0; i < ballCount; ++i) {
      Serial.print("Ball #");
      Serial.print(i);
      Serial.print(": Force of ");
      Serial.print(addedForce);
      Serial.print("J changes speed from ");
      Serial.print(balls[i].speed, 4);
      Serial.print(" to ");
      balls[i].applyForce(addedForce);
      Serial.print(balls[i].speed, 4);
      Serial.println("m/s");
    }
  } else {
    Serial.read();
  }
}

void renderDots() {
  // Render the height of the ball on the line.
  for (byte i = ballCount; i-- > 0;) {
    byte intensity = min(127, balls[i].speed * 25);
    strip.setPixelColor(balls[i].height / ledSpacing,
                        127 - intensity, 0, intensity);
  }
  strip.show();
  for (byte i = ballCount; i-- > 0;)
    strip.setPixelColor(balls[i].height / ledSpacing, 0);
}
