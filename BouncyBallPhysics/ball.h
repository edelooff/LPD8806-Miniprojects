enum Direction {
  DOWN = 1,
  UP = -1
};

class Ball {
  public:
    Ball(float ceiling, int mass, float elasticity),
    Ball(float ceiling, int mass, float elasticity, float height);
    const float elasticity;
    float
      height,
      speed,
      kineticEnergy(),
      velocity();
    void
      accelerate(float acceleration, float duration),
      applyForce(float energy, Direction dir),
      applyForce(float energy),
      report(void),
      travel(float duration);

  private:
    const int mass;
    const float ceiling;
    Direction direction;
    float
      acceleratedMovement(float acceleration, float duration),
      accelerationTime(float acceleration, float distance);
    void
      applyTemporalForce(float acceleration, float duration),
      bounce(),
      reverseDirection(),
      speedFromEnergy(float energy);
};
