#include "ball.h"

Ball::Ball(float ceiling, int mass, float elasticity):
    ceiling(ceiling),
    mass(mass),
    elasticity(elasticity) {
        direction = DOWN;
}

Ball::Ball(float ceiling, int mass, float elasticity, float height, float speed):
    ceiling(ceiling),
    mass(mass),
    elasticity(elasticity),
    height(height),
    speed(speed) {
        direction = DOWN;
}

void Ball::accelerate(float acceleration, float duration) {
  // Applies an acceleration on the ball.
  //
  // Gravity and other downward pulling forces should
  // be given as positive numbers. Similarly, upward
  // forces should have a negative sign.
  float distance, newHeight, timeTaken;
  if (speed || height > 0) {
    newHeight = height - displacementAccelerated(acceleration, duration);
    if (newHeight < 0 || newHeight > ceiling) {
      distance = newHeight < 0 ? height : ceiling - height;
      // Bounce on the floor, calculate exact collision time
      if (!distance) {
        timeTaken = 0;
      } else {
        // pre-bounce only applies if there is distance to cover.
        Serial.print("Remaining distance: ");
        Serial.print(distance, 4);
        Serial.print(" is covered in: ");
        timeTaken = timeForDisplacement(acceleration, distance);
        Serial.println(timeTaken, 4);
        changeVelocity(acceleration, timeTaken);
      }
      bounce();
      changeVelocity(acceleration, duration - timeTaken); // post-bounce
      height = newHeight < 0 ? 0 : ceiling;
      height -= displacementAccelerated(acceleration, duration - timeTaken);
      if (height < 0) {
        height = 0;
        speed = 0;
      }
      Serial.print("post bounce: ");
      serialReport();
    } else {
      height = newHeight;
      changeVelocity(acceleration, duration);
    }
  }
}

void Ball::applyForce(float force) {
  // Applies a certain force to the ball in the current direction of motion.
  applyForce(force, direction);
}

void Ball::applyForce(float force, Direction dir) {
  // Applies a certain force to the ball in the given direction.
  //
  // If the force applied is in the current direction of motion,
  // the ball simply speeds up. In the other case, the ball slows
  // down until it hits a dead stop. If the force is greater than
  // that, the direction is reversed and the remainder of the force
  // is applied to generate speed in the new direction.
  if (dir == direction) {
    speedFromEnergy(kineticEnergy() + force);
  } else {
  float currentEnergy = kineticEnergy();
    if (force > currentEnergy)
      reverseDirection();
    speedFromEnergy(abs(currentEnergy - force));
  }
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
  serialReport();
  reverseDirection();
  speedFromEnergy(kineticEnergy() * elasticity);
}

void Ball::changeVelocity(float acceleration, float duration) {
  // Changes the velocity of the ball resulting from acceleration
  float speedDiff = abs(acceleration * duration);
  if (direction == (acceleration >= 0 ? DOWN : UP)) {
    speed += speedDiff;
  } else if (speedDiff > speed) {
    speed = speedDiff - speed;
    reverseDirection();
  } else {
    speed -= speedDiff;
  }
}

void Ball::travel(float duration) {
  // Moves the ball for a given duration at the current velocity
  height += duration * velocity();
}

void Ball::reverseDirection(void) {
  // Reverses the direction of movement of the ball.
  direction = direction == UP ? DOWN : UP;
}

void Ball::speedFromEnergy(float energy) {
  // Resets the speed of the ball based on its mass and given kinetic energy
  speed = sqrt(energy / mass * 2);
}

float Ball::displacementAccelerated(float acceleration, float duration) {
  // Returns the distance moved during acceleration
  return displacementConstant(duration) + 0.5 * acceleration * sq(duration);
}

float Ball::displacementConstant(float duration) {
  // Returns thh distance moved during constant velocity
  return velocity() * duration;
}

float Ball::timeForDisplacement(float acceleration, float distance) {
  // Returns the time needed to travel the given distance under acceleration
  return (sqrt(sq(speed) + 2 * acceleration * distance) - speed) / acceleration;
}

float Ball::kineticEnergy(void) {
  // Returns the kinetic energy of the ball in Joule.
  return 0.5 * mass * sq(speed);
}

float Ball::velocity(void) {
  // Returns the velocity (speed * direction) of the ball; Positive is downward
  return speed * direction;
}

void Ball::serialReport() {
  // Reports the current height, speed and kinetic energy over serial
  Serial.print("h: ");
  Serial.print(height, 4);
  Serial.print(", v: ");
  Serial.print(speed, 4);
  Serial.print(", k: ");
  Serial.println(kineticEnergy(), 2);
}
