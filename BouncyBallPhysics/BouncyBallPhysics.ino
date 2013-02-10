#include <LPD8806.h>
#include <SPI.h>
#include "ball.h"

const int
  ledCount = 158,
  ledPerMeter = 52,
  updateInterval = 3;
const float
  gravity = 9.807, // Earth gravity in m/s^2
//  gravity = 1.622, // Lunar gravity in m/s^2
  ledSpacing = 1.0 / ledPerMeter,
  stripCeiling = (ledCount - 1) * ledSpacing,
  timeFactor = updateInterval / 1000.0;
LPD8806
  strip = LPD8806(ledCount);
Ball balls[] = {
    Ball(stripCeiling, 25, .80, 1.2),
    Ball(stripCeiling, 15, .75, 1.2),
    Ball(stripCeiling, 10, .65, 1.5)};
const long colors[] = {
    strip.Color(127, 0, 0),
    strip.Color(0, 127, 0),
    strip.Color(0, 0, 127)};
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
      balls[i].travel(timeFactor, gravity);
    renderDots();
  }
}

void serialAddKineticEnergy(void) {
  byte peekChar = Serial.peek();
  if (peekChar >= '0' && peekChar <= '9') {
    // Adds or subtracts kinetic energy to/from the current ball motion.
    unsigned long addedForce = Serial.parseInt();
    for (int i = 0; i < ballCount; ++i) {
      Serial.print("Ball #");
      Serial.print(i + 1);
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
    if (Serial.read() == '?') {
      for (int i = 0; i < ballCount; ++i) {
        Serial.print("Ball #");
        Serial.print(i + 1);
        Serial.print(": ");
        balls[i].serialReport();
      }
    }
  }
}

void renderDots() {
  // Render the height of the ball on the line.
  for (byte i = 0; i < ballCount; ++i) {
    strip.setPixelColor((balls[i].height + ledSpacing / 2) / ledSpacing, colors[i]);
  }
  strip.show();
  for (byte i = ballCount; i-- > 0;)
    strip.setPixelColor((balls[i].height + ledSpacing / 2) / ledSpacing, 0);
}
