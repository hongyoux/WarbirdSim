# include "Missile.h"
# include "WarBird.h"
# include "MissileBase.h"
# include <stdint.h>
# include <iostream>

namespace
{
  bool colinear(glm::vec3 v1, glm::vec3 v2, double epsilon) {
    glm::vec3 v1t, v2t; // local "temp" vectors
    v1t = glm::abs(glm::normalize(v1));
    v2t = glm::abs(glm::normalize(v2));
    if (glm::distance(v1t, v2t) <= epsilon) return true;
    else return false;
  }
}

Missile::Missile(glm::vec4 initialLocation)
  : SimObject("Models/Missile.tri"
    , initialLocation
    , 25
    , 0.0f
    , 240 * 3)
  , mState(MISSILE_STATE::WAITING)
  , mFrameCount(0)
  {}

Missile::~Missile()
{
  glDeleteVertexArrays(1, &mVao);
  glDeleteBuffers(1, &mBuffer);
}

void Missile::UpdateState(std::vector<SimObject*> entities)
{
  switch (mState)
  {
  case MISSILE_STATE::WAITING:
    UpdateWaiting();
    break;
  case MISSILE_STATE::SEARCHING:
    UpdateSearching(entities);
    break;
  case MISSILE_STATE::SEEKING:
    UpdateSeeking();
    break;
  case MISSILE_STATE::DEAD:
    //DO NOTHING
    break;
  }
}

void Missile::UpdateWaiting()
{
  UpdateMovement();
  if (mFrameCount > kCOOKING_PERIOD)
  {
    mState = MISSILE_STATE::SEARCHING;
  }
}

void Missile::UpdateSearching(std::vector<SimObject*> entities)
{
  UpdateMovement();
}

void Missile::UpdateSeeking()
{
  TrackTarget();
  UpdateMovement();
  CheckCollision();
}

void Missile::UpdateMovement()
{
  mLocation = mLocation + mModelRotationMatrix[2] * mVelocity;
  mFrameCount++;
  if (mFrameCount > kLIFESPAN)
  {
    mState = MISSILE_STATE::DEAD;
  }
}

void Missile::TrackTarget()
{
  OrientToTarget(mTarget);
}

void WarBirdMissile::UpdateSearching(std::vector<SimObject*> entities)
{
  Missile::UpdateSearching(entities);
  float closestMissileSilo = FLT_MAX;
  MissileSilo* target = nullptr;
  for (SimObject* obj : entities)
  {
    UnumMissileSilo* unumMisSilo = dynamic_cast<UnumMissileSilo*>(obj);
    SecundusMissileSilo* secundusMisSilo = dynamic_cast<SecundusMissileSilo*>(obj);

    if (unumMisSilo != nullptr && !unumMisSilo->IsDead())
    {
      //Check distance from missile
      float distance = glm::distance(obj->mLocation, mLocation);
      if (distance < kDETECTION_RADIUS && distance < closestMissileSilo)
      {
        target = unumMisSilo;
      }
    }
    if (secundusMisSilo != nullptr && !secundusMisSilo->IsDead())
    {
      //Check distance from missile
      float distance = glm::distance(obj->mLocation, mLocation);
      if (distance < kDETECTION_RADIUS && distance < closestMissileSilo)
      {
        target = secundusMisSilo;
      }
    }
  }
  if (target != nullptr)
  {
    mTarget = target;
    mState = MISSILE_STATE::SEEKING;
  }
}

void WarBirdMissile::CheckCollision()
{
  MissileSilo* misSilo = dynamic_cast<MissileSilo*>(mTarget);
  // Center to center distance
  float distance = glm::distance(mTarget->mLocation, mLocation);
  // Min bounding sphere distance
  float boundingDistance = mTarget->mRadius + mRadius + 10;
  if (distance < boundingDistance) // Collision detected!
  {
    mState = MISSILE_STATE::DEAD;
    misSilo->SetDead(true);
  }
}

void BaseMissile::UpdateSearching(std::vector<SimObject*> entities)
{
  Missile::UpdateSearching(entities);
  float closestMissileSilo = FLT_MAX;
  SimObject* target = nullptr;
  for (SimObject* obj : entities)
  {
    bool isBird = dynamic_cast<WarBird*>(obj) != nullptr;

    if (isBird)
    {
      //Check distance from missile
      float distance = glm::distance(obj->mLocation, mLocation);
      if (distance < kDETECTION_RADIUS && distance < closestMissileSilo)
      {
        mTarget = obj;
        mState = MISSILE_STATE::SEEKING;
        return;
      }
    }
  }
}

void BaseMissile::CheckCollision()
{
  WarBird* ship = dynamic_cast<WarBird*>(mTarget);
  // Center to center distance
  float distance = glm::distance(mTarget->mLocation, mLocation);
  // Min bounding sphere distance
  float boundingDistance = mTarget->mRadius + mRadius + 10;
  if (distance < boundingDistance) // Collision detected!
  {
    mState = MISSILE_STATE::DEAD;
    std::cout << "TARGET HIT";
  }
}
