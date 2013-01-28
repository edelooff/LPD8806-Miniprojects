#include <LPD8806.h>
#include <SPI.h>
#include "ball.h"

const int
  deadSpeed = 5,
  ledCount = 158,
  ledPerMeter = 52,
  updateInterval = 2;
const float
  gravity = 0.981, // Gravitational acceleration in mm/[timeBase]^2
  ledSpacing = 1000 / ledPerMeter,
  stripCeiling = (ledCount - 1) * ledSpacing,
  timeBase = 10,   // timebase for all velocity/acceleration units (in ms)
  timeFactor = updateInterval / timeBase;
LPD8806
  strip = LPD8806(ledCount);
ball_t
  ball = {1, 0, 500, 0.8};

void setup() {
  strip.begin();
  strip.show();
  Serial.begin(9600);
}

void loop() {
  static long nextStep = 0;
  long milliSeconds = millis();
  if (Serial.available())
    ball.velocity += processSerial();
  if (milliSeconds >= nextStep) {
    nextStep = milliSeconds + updateInterval;
    calculatePhysics();
    renderDot(ball.height / ledSpacing);
  }
}

void calculatePhysics(void) {
  if (ball.velocity || ball.height > 0) {
    ball.height -= ball.velocity * timeFactor;
    ball.height -= gravity / 2 * sq(timeFactor);
    ball.velocity += gravity * timeFactor;
  }
  if (ball.height < 0) {
    ball.height = 0;
    if (abs(ball.velocity) < deadSpeed) {
      ball.velocity = 0;
    } else {
      ball.velocity = -postBounceVelocity(ball);
    }
  } else if (ball.height > stripCeiling) {
    ball.height = stripCeiling;
    ball.velocity = postBounceVelocity(ball);
  }
}

float postBounceVelocity(ball_t ball) {
  // Calculates the velocity after the bounce
  //
  // Calculates the current kinetic energy and multiplies this
  // with the ball's elasticity. The resulting energy is used to
  // calculate the new velocity.
  float energy = kineticEnergy() * ball.elasticity;
  return speedFromKinetic(energy);
}

float kineticEnergy(void) {
  // Returns the kinetic energy in Joules.
  return ball.mass * sq(ball.velocity / 10);
}

float speedFromKinetic(float kinetic) {
  // Given kinetic energy in Joule, how fast will the
  // ball move, in millimeters per [timeBase]
  return sqrt(kinetic / ball.mass) * 10;
}

int processSerial(void) {
  byte sP = Serial.peek();
  if ((sP >= '0' && sP <= '9') || sP == '-')
    // Process this as extra kinetic energy, not direct speed
    return Serial.parseInt();
  else
    Serial.read();
  return 0;
}

void renderDot(int position) {
  // Render the height of the ball on the line.
  byte intensity = min(127, abs(ball.velocity) * 4);
  strip.setPixelColor(position, 127 - intensity, 0, intensity);
  strip.show();
  strip.setPixelColor(position, 0);
}
