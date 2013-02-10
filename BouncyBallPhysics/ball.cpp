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

void Ball::travel(float duration, float gravity) {
  // Applies an acceleration on the ball.
  //
  // Gravity and other downward pulling forces should
  // be given as positive numbers. Similarly, upward
  // forces should have a negative sign.
  float distance, newHeight, timeTaken, acceleration = -gravity;
  if (speed || (height < ceiling && height > 0)) {
    newHeight = height + displacementAccelerated(duration, acceleration);
    if (newHeight <= 0 || newHeight >= ceiling) {
      // Ball bounces, calculate exact time to collision
      #if DEBUG
        Serial.print("Bounce! d-calculated=");
        Serial.println(displacementAccelerated(duration, acceleration), 7);
      #endif
      distance = newHeight < 0 ? -height : ceiling - height;
      if (!distance) {
        timeTaken = 0;
      } else {
        // Only calculate time required if there is distance to cover
        #if DEBUG
          Serial.print("d-to-impact=");
          Serial.print(distance, 7);
          Serial.print(", v=");
          Serial.print(velocity(), 7);
        #endif
        timeTaken = timeForDisplacement(distance, acceleration);
        changeVelocity(timeTaken, acceleration);
        #if DEBUG
          Serial.print(", delta-t=");
          Serial.println(timeTaken, 7);
        #endif
      }
      bounce();
      // After the bounce, calculate the post-bounce movement
      changeVelocity(duration - timeTaken, acceleration);
      height = newHeight < 0 ? 0 : ceiling;
      #if DEBUG
        Serial.print("t-post-bounce=");
        Serial.print(duration - timeTaken, 7);
        Serial.print(", v=");
        Serial.print(velocity(), 7);
        Serial.print(", delta-h=");
        Serial.println(
            displacementAccelerated(duration - timeTaken, acceleration), 6);
      #endif
      height += displacementAccelerated(duration - timeTaken, acceleration);
      postBounceLimitCheck();
    } else {
      height = newHeight;
      changeVelocity(duration, acceleration);
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
  #if DEBUG
    if (height > ceiling / 2)
      Serial.print("Ceiling bounce: ");
    else
      Serial.print("Floor bounce: ");
    serialReport();
  #endif
  reverseDirection();
  speedFromEnergy(kineticEnergy() * elasticity);
}

void Ball::postBounceLimitCheck(void) {
  // Checks whether the ball is within bounds after moving post-bounce.
  //
  // If it's out, it's placed at the edge with no speed, facing inward.
  if (height < 0) {
    height = 0;
    speed = 0;
    direction = UP;
  } else if (height > ceiling) {
    height = ceiling;
    speed = 0;
    direction = DOWN;
  }
}

void Ball::changeVelocity(float duration, float acceleration) {
  // Changes the velocity of the ball resulting from acceleration
  float speedDiff = abs(acceleration * duration);
  if (direction == (acceleration <= 0 ? DOWN : UP)) {
    speed += speedDiff;
  } else if (speedDiff > speed) {
    speed = speedDiff - speed;
    reverseDirection();
  } else {
    speed -= speedDiff;
  }
}

void Ball::reverseDirection(void) {
  // Reverses the direction of movement of the ball.
  direction = direction == UP ? DOWN : UP;
}

void Ball::speedFromEnergy(float energy) {
  // Resets the speed of the ball based on its mass and given kinetic energy
  speed = sqrt(energy / mass * 2);
}

float Ball::displacementAccelerated(float duration, float acceleration) {
  // Returns the distance moved during acceleration
  return displacementConstant(duration) + 0.5 * acceleration * sq(duration);
}

float Ball::displacementConstant(float duration) {
  // Returns thh distance moved during constant velocity
  return velocity() * duration;
}

float Ball::timeForDisplacement(float distance, float acceleration) {
  // Returns the time needed to travel the given distance under acceleration
  if (!acceleration)
    return distance / speed;
  return (sqrt(sq(speed) + 2 * acceleration * distance) - speed) / acceleration;
}

float Ball::kineticEnergy(void) {
  // Returns the kinetic energy of the ball in Joule.
  return 0.5 * mass * sq(speed);
}

float Ball::velocity(void) {
  // Returns the velocity (speed * direction) of the ball; Positive is upward
  return speed * direction;
}

void Ball::serialReport() {
  // Reports the current height, speed and kinetic energy over serial
  Serial.print("h=");
  Serial.print(height, 7);
  Serial.print(", v=");
  Serial.print(speed, 7);
  Serial.print(", k=");
  Serial.println(kineticEnergy(), 4);
}
