#pragma once

#include "SimObject.h"
#include "Missile.h"
#include <vector>

class WarBird : public SimObject
{
public:
  enum ThrusterType
  {
    SLOW,
    MEDIUM,
    FAST
  };

  WarBird();

  void UpdateMovement(bool isGravityOn);
  void CheckForCollisions(std::vector<SimObject*>& entities);

  void UseNextThruster();
  void SetKeysPressed(int key, bool isCtrlActive);
  void SetKeysReleased(int key, bool isCtrlActive);

  void SetMissile(Missile* missile);
  bool HasMissile() { return mMissile != nullptr; };

  bool IsDead() { return mIsDead; }

  Missile* mMissile;

private:
  float GetShipSpeed();

  void UpdateThrusters();
  ThrusterType mThrusterType;

  /// Thrusters "off" = 0
  /// Thrusters "on" forward = 1
  /// Thrusters "on" backwards = -1
  int yawThruster;
  int pitchThruster;
  int rollThruster;
  int forwardThruster;

  /// Key managers to support pressing multiple keys
  int mArrowKeysPressed[4];
  int mSpecialArrowsPressed[4];

  const int gravityFactor = 90000000;

  bool mIsDead = false;
};
