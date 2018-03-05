#include "MissileBase.h"

#include <algorithm>

MissileSilo::MissileSilo(std::string name)
  : SimObject(name
    , glm::vec4(0.0f, -30.0f, 0.0f, 0.0f)
    , 30
    , 0.0f
    , 48 * 3)
{
  mMissileCount = 5;
  mWindupCount = 0;
}

void MissileSilo::UpdateState(std::vector<SimObject*> entities)
{
  mWindupCount++;
  if (mWindupCount > kSTART)
  {
    mReady = true;
  }
}

bool MissileSilo::FoundShip(SimObject * ship)
{
  float shipDistance = glm::distance(mLocation, ship->mLocation);
  if (shipDistance < kDETECT_RANGE)
  {
    return true;
  }
  return false;
}

void MissileSilo::SetMissile(Missile * missile)
{
  mMissile = missile;
  mMissileCount -= 1;
}

UnumMissileSilo::UnumMissileSilo()
  : MissileSilo("Models/UnumMissileSilo.tri")
{
}

SecundusMissileSilo::SecundusMissileSilo()
  : MissileSilo("Models/SecundusMissileSilo.tri")
{
}
