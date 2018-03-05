#include "WarBird.h"
#include <algorithm>

WarBird::WarBird()
  : SimObject("Models/Warbird.tri"
    , glm::vec4(15000.0f, 0.0f, 0.0f, 0.0f)
    , 100
    , 0.0f
    , 84 * 3)
  , forwardThruster(0)
  , yawThruster(0)
  , pitchThruster(0)
  , rollThruster(0)
  , mArrowKeysPressed{ 0 }
  , mSpecialArrowsPressed{ 0 }
  , mMissile(nullptr)
  , mThrusterType(ThrusterType::SLOW)
{
  mMissileCount = 9;
}

void WarBird::UpdateMovement(bool isGravityOn)
{
  if (IsDead())
  {
    return;
  }
  UpdateThrusters();
  
  ///Update Ship model rotation
  glm::mat4 yaw = glm::rotate(glm::mat4(), yawThruster * .02f, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 pitch = glm::rotate(glm::mat4(), pitchThruster * .02f, glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 roll = glm::rotate(glm::mat4(), rollThruster * .02f, glm::vec3(0.0f, 0.0f, -1.0f));
  mModelRotationMatrix = mModelRotationMatrix * yaw * pitch * roll;

  float distanceFromRuber = glm::pow(glm::distance(glm::vec4(0), mLocation), 2);
  glm::vec4 directionToCenter = glm::normalize(glm::vec4(0) - mLocation);

  glm::vec4 gravityVec(0);
  if (isGravityOn)
  {
    gravityVec = directionToCenter * (gravityFactor / distanceFromRuber);
  }
  glm::vec4 speedVec = (mModelRotationMatrix[2] * (forwardThruster * GetShipSpeed()));

  ///Move ship in direction of ship if mLocation is on.
  mLocation = mLocation + gravityVec + speedVec;
}

void WarBird::CheckForCollisions(std::vector<SimObject*>& entities)
{
  if (IsDead())
  {
    return;
  }

  for (SimObject* obj : entities)
  {
    if (obj == this)
    {
      continue;
    }
    Missile* missile = dynamic_cast<Missile*>(obj);
    if (missile != nullptr && missile->GetMissileState() == Missile::MISSILE_STATE::WAITING)
    {
      continue;
    }

    // Center to center distance
    float distance = glm::distance(obj->mLocation, mLocation);
    // Min bounding sphere distance
    float boundingDistance = obj->mRadius + mRadius + 10;
    if (distance < boundingDistance) // Collision detected!
    {
      mIsDead = true;

      entities.erase(std::remove(entities.begin(), entities.end(), this), entities.end());
      return;
    }
  }
}

void WarBird::UseNextThruster()
{
  mThrusterType = static_cast<ThrusterType>((mThrusterType + 1) % 3);
}

void WarBird::SetKeysPressed(int key, bool isCtrlActive)
{
  /// 0 is up, 1 is down, 2 is left, 3 is right
  if (isCtrlActive)
  {
    switch (key)
    {
    case GLUT_KEY_UP: // ship pitches "down"
      mSpecialArrowsPressed[0] = 1;
      break;
    case GLUT_KEY_DOWN: // ship pitches "up"
      mSpecialArrowsPressed[1] = -1;
      break;
    case GLUT_KEY_LEFT: // ship rolls "right"
      mSpecialArrowsPressed[2] = 1;
      break;
    case GLUT_KEY_RIGHT: // ship rolls "left"
      mSpecialArrowsPressed[3] = -1;
      break;
    }
  }
  else
  {
    switch (key)
    {
    case GLUT_KEY_UP: // ship forwards
      mArrowKeysPressed[0] = 1;
      break;
    case GLUT_KEY_DOWN: // ship backwards
      mArrowKeysPressed[1] = -1;
      break;
    case GLUT_KEY_LEFT: // ship yaws "left"
      mArrowKeysPressed[2] = 1;
      break;
    case GLUT_KEY_RIGHT: // ship yaws "right"
      mArrowKeysPressed[3] = -1;
      break;
    }
  }
}

void WarBird::SetKeysReleased(int key, bool isCtrlActive)
{
  switch (key)
  {
  case GLUT_KEY_UP: // ship forwards
    mArrowKeysPressed[0] = 0;
    mSpecialArrowsPressed[0] = 0;
    break;
  case GLUT_KEY_DOWN: // ship backwards
    mArrowKeysPressed[1] = 0;
    mSpecialArrowsPressed[1] = 0;
    break;
  case GLUT_KEY_LEFT: // ship yaws "right"
    mArrowKeysPressed[2] = 0;
    mSpecialArrowsPressed[2] = 0;
    break;
  case GLUT_KEY_RIGHT: // ship yaws "left"
    mArrowKeysPressed[3] = 0;
    mSpecialArrowsPressed[3] = 0;
    break;
  }
}

void WarBird::SetMissile(Missile* missile) {
  mMissile = missile;
  mMissileCount--;
}

void WarBird::UpdateThrusters()
{
  /// If you press both keys, the ship will stop moving.
  forwardThruster = mArrowKeysPressed[0] + mArrowKeysPressed[1];
  yawThruster = mArrowKeysPressed[2] + mArrowKeysPressed[3];
  pitchThruster = mSpecialArrowsPressed[0] + mSpecialArrowsPressed[1];
  rollThruster = mSpecialArrowsPressed[2] + mSpecialArrowsPressed[3];
}

float WarBird::GetShipSpeed()
{
  switch (mThrusterType)
  {
  default:
  case ThrusterType::SLOW:
    return 10.0f;
  case ThrusterType::MEDIUM:
    return 50.0f;
  case ThrusterType::FAST:
    return 100.0f;
  }
}