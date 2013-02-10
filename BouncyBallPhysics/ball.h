#include <Arduino.h>

class Ball {
  public:
    enum Direction {
        DOWN = 1,
        UP = -1
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
      accelerate(float acceleration, float duration),
      applyForce(float force),
      applyForce(float force, Direction dir),
      serialReport(),
      travel(float duration);

  private:
    const int mass;
    const float ceiling;
    Direction direction;
    float
      displacementAccelerated(float acceleration, float duration),
      displacementConstant(float duration),
      timeForDisplacement(float acceleration, float distance);
    void
      bounce(void),
      changeVelocity(float acceleration, float duration),
      postBounceLimitCheck(void),
      reverseDirection(void),
      speedFromEnergy(float energy);
};
