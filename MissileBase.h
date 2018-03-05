#pragma once

#include "SimObject.h"
#include "Missile.h"
#include <string>
#include <vector>

class MissileSilo : public SimObject
{
public:
  MissileSilo(std::string name);

  void UpdateState(std::vector<SimObject*> entities);

  void SetDead(bool dead) { mIsDead = true; }
  bool IsDead() { return mIsDead; }
  bool FoundShip(SimObject* ship);

  bool HasMissile() { return mMissile != nullptr; }
  void SetMissile(Missile* missile);
  Missile* mMissile;

  bool IsReady() { return mReady; }

private:
  bool mIsDead;

  int kDETECT_RANGE = 3000;
  int kSTART = 100;
  int mWindupCount = 0;
  bool mReady = false;
};

class UnumMissileSilo : public MissileSilo
{
public:
  UnumMissileSilo();
};

class SecundusMissileSilo : public MissileSilo
{
public:
  SecundusMissileSilo();
};

