#include <LPD8806.h>
#include <SPI.h>
#include "ball.h"

const int
  ledCount = 158,
  ledPerMeter = 52,
  updateInterval = 2;
const float
  deadSpeed = .2, // Speeds under 0.2m/s will cause the ball to fall dead
  gravity = 9.807, // Earth gravity in m/s^2
//  gravity = 1.622, // Lunar gravity in m/s^2
  ledSpacing = 1.0 / ledPerMeter,
  stripCeiling = (ledCount - 1) * ledSpacing,
  timeFactor = updateInterval / 1000.0;
LPD8806
  strip = LPD8806(ledCount);
ball_t
  ball = {5, 0, 1, .75};

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
    Serial.print("Floor bounce @ ");
    Serial.print(ball.velocity, 4);
    Serial.println("m/s");
    ball.height = 0;
    if (abs(ball.velocity) < deadSpeed)
      ball.velocity = 0;
    else
      ball.velocity = -postBounceVelocity(ball);
  } else if (ball.height > stripCeiling) {
    Serial.print("Ceiling bounce @ ");
    Serial.print(ball.velocity, 4);
    Serial.println("m/s");
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
  return ball.mass * sq(ball.velocity);
}

float speedFromKinetic(float energy) {
  // Given kinetic energy in Joule, how fast will the
  // ball move, in millimeters per [timeBase]
  return sqrt(energy / ball.mass);
}

void serialAddKineticEnergy(void) {
  float newVelocity;
  int addedForce;
  byte peekChar = Serial.peek();
  if ((peekChar >= '0' && peekChar <= '9') || peekChar == '-') {
    // Adds or subtracts kinetic energy to/from the current ball motion.
    addedForce = Serial.parseInt();
    Serial.print("Force of ");
    Serial.print(addedForce);
    Serial.print("J changes speed from ");
    Serial.print(ball.velocity, 4);
    Serial.print(" to ");
    newVelocity = speedFromKinetic(max(0, kineticEnergy() + addedForce)); 
    ball.velocity = (ball.velocity > 0) ? newVelocity : -newVelocity;
    Serial.print(ball.velocity, 4);
    Serial.println("m/s");
  } else {
    Serial.read();
  }
}

void renderDot(int position) {
  // Render the height of the ball on the line.
  byte intensity = min(127, abs(ball.velocity) * 25);
  strip.setPixelColor(position, 127 - intensity, 0, intensity);
  strip.show();
  strip.setPixelColor(position, 0);
}
