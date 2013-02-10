#include <Arduino.h>

class Ball {
  public:
    enum Direction {
        DOWN = -1,
        UP = 1
    };
    Ball(float ceiling, int mass, float elasticity),
    Ball(float ceiling, int mass, float elasticity, float height, float speed = 0);
    const float elasticity;
    float
      height,
      speed,
      kineticEnergy(void),
      velocity(void);
    void
      applyForce(float force),
      applyForce(float force, Direction dir),
      serialReport(),
      travel(float duration, float acceleration = 0);

  private:
    const int mass;
    const float ceiling;
    Direction direction;
    float
      displacementAccelerated(float duration, float acceleration),
      displacementConstant(float duration),
      timeForDisplacement(float distance, float acceleration);
    void
      bounce(void),
      changeVelocity(float acceleration, float duration),
      postBounceLimitCheck(void),
      reverseDirection(void),
      speedFromEnergy(float energy);
};
