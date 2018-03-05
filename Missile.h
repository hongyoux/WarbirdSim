#pragma once

#include "SimObject.h"
#include <vector>

class Missile : public SimObject
{
public:
  enum MISSILE_STATE
  {
    WAITING,
    SEARCHING,
    SEEKING,
    DEAD
  };

  Missile(glm::vec4 initialLocation);
  virtual ~Missile();

  MISSILE_STATE GetMissileState() { return mState; }
  void UpdateState(std::vector<SimObject*> entities);

protected:
  void UpdateWaiting();
  virtual void UpdateSearching(std::vector<SimObject*> entities);
  void UpdateSeeking();
  void UpdateMovement();
  void TrackTarget();
  virtual void CheckCollision() = 0;

  MISSILE_STATE mState;
  uint32_t mFrameCount;

  uint32_t kCOOKING_PERIOD = 50;
  uint32_t kLIFESPAN = 1000;
  uint32_t kMAX_DISTANCE = 10000;
  uint32_t kDETECTION_RADIUS = 3000;

  float mVelocity;
  SimObject* mTarget;
};

class WarBirdMissile : public Missile
{
public:
  WarBirdMissile(glm::vec4 initialLocation)
    : Missile(initialLocation)
  {
    mVelocity = 35.0f;
  }

  void UpdateSearching(std::vector<SimObject*> entities) override;
  void CheckCollision() override;
};

class BaseMissile : public Missile
{
public:
  BaseMissile(glm::vec4 initialLocation)
    : Missile(initialLocation)
  {
    mVelocity = 25.0f;
  }

  void UpdateSearching(std::vector<SimObject*> entities) override;
  void CheckCollision() override;
};