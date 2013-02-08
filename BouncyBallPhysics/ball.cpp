#include "ball.h"
#include <Arduino.h>

Ball::Ball(float ceiling, int mass, float elasticity):
    ceiling(ceiling),
    mass(mass),
    elasticity(elasticity) {
}

Ball::Ball(float ceiling, int mass, float elasticity, float height):
    ceiling(ceiling),
    mass(mass),
    elasticity(elasticity),
    height(height) {
}

void Ball::accelerate(float acceleration, float duration) {
  // Applies an acceleration on the ball.
  //
  // Gravity and other downward pulling forces should
  // be given as positive numbers. Similarly, upward
  // forces should have a negative sign.
  float distance, newHeight, timeTaken;
  if (speed || height > 0) {
    newHeight = height - acceleratedMovement(acceleration, duration);
    if (newHeight < 0 || newHeight > ceiling) {
      distance = (newHeight < 0) ? height : ceiling - height;
      // Bounce on the floor, calculate exact collision time
      if (!distance) {
        timeTaken = 0;
      } else {
        // pre-bounce only applies if there is distance to cover.
        Serial.print("Remaining distance: ");
        Serial.print(distance, 4);
        Serial.print(" is covered in: ");
        timeTaken = accelerationTime(acceleration, distance);
        Serial.println(timeTaken, 4);
        applyTemporalForce(acceleration, timeTaken);
      }
      bounce();
      applyTemporalForce(acceleration, duration - timeTaken); // post-bounce
      height = (newHeight < 0) ? 0 : ceiling;
      height -= acceleratedMovement(acceleration, duration - timeTaken);
      if (height < 0) {
        height = 0;
        speed = 0;
      }
      Serial.print("post bounce: ");
      report();
    } else {
      height = newHeight;
      applyTemporalForce(acceleration, duration);
    }
  }
}

void Ball::applyTemporalForce(float acceleration, float duration) {
  // Applies a force to the ball resulting from acceleration for a given time
  applyForce(abs(mass * acceleration * duration), acceleration > 0 ? DOWN : UP);
}

void Ball::applyForce(float energy, Direction dir) {
  // Applies a certain force to the ball in the given direction.
  //
  // If the force applied is in the current direction of motion,
  // the ball simply speeds up. In the other case, the ball slows
  // down until it hits a dead stop. If the force is greater than
  // that, the direction is reversed and the remainder of the force
  // is applied to generate speed in the new direction.
  if (dir == direction) {
    speedFromEnergy(kineticEnergy() + energy);
  } else {
  float currentEnergy = kineticEnergy();
    if (energy > currentEnergy)
      reverseDirection();
    speedFromEnergy(abs(currentEnergy - energy));
  }
}

void Ball::applyForce(float energy) {
  // Applies a certain force to the ball in the current direction of motion.
  speedFromEnergy(kineticEnergy() + energy);
}

void Ball::bounce(void) {
  // Bounces the ball, flipping direction of movement.
  //
  // Also chips away a fraction of the kinetic energy of the ball,
  // dependent on the elasticity factor.
  if (height > ceiling / 2)
    Serial.print("Ceiling bounce @ ");
  else
    Serial.print("Floor bounce @ ");
  report();
  reverseDirection();
  speedFromEnergy(max(0, kineticEnergy() - 1) * elasticity);
}

void Ball::travel(float duration) {
  // Moves the ball for a given duration at the current velocity;
  //
  // This effectively moves the ball in a zero-g environment,
  // with no gravitational pull acting on it.
  height += duration * velocity();
}

void Ball::reverseDirection(void) {
  // Reverses the direction of movement of the ball.
  direction = direction == UP ? DOWN : UP;
}

void Ball::speedFromEnergy(float energy) {
  // Given the kinetic energy and mass
  // of the object, calculate its speed:
  speed = sqrt(energy / mass * 2);
}

float Ball::acceleratedMovement(float acceleration, float duration) {
  // Returns the distance moved during acceleration
  return velocity() * duration + acceleration / 2 * sq(duration);
}

float Ball::accelerationTime(float acceleration, float distance) {
  // Returns the time needed to travel the given distance
  //
  // The time required is dependent on both the current speed and the
  // acceleration force acting on the ball.
  return (sqrt(sq(speed) + 2 * acceleration * distance) - speed) / acceleration;
}

float Ball::kineticEnergy(void) {
  // Returns the kinetic energy of the ball in Joule.
  return 0.5 * mass * sq(speed);
}

float Ball::velocity(void) {
  // Returns the velocity of the ball (speed * direction).
  //
  // Our system only has two directions, up and down. These are
  // represented by negative and positive velocity respectively.
  return speed * direction;
}

void Ball::report() {
  Serial.print("h: ");
  Serial.print(height, 4);
  Serial.print(", v: ");
  Serial.print(speed, 4);
  Serial.print(", k: ");
  Serial.println(kineticEnergy(), 2);
}
