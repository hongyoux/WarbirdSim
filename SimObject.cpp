#include "SimObject.h"
#include <string>

namespace
{
  ///COPIED FROM glmUtils465 because could not figure out how to import 
  ///includes465 multiple times

  // v1 and v2 are colinear within an epsilon (rounding) range
  // epislon of 0.0 has no range, suggest 0.1 for orient towards
  bool colinear(glm::vec3 &v1, glm::vec3 &v2, double epsilon) {
    glm::vec3 v1t, v2t;
    v1t = glm::abs(glm::normalize(v1));
    v2t = glm::abs(glm::normalize(v2));
    if (glm::distance(v1t, v2t) <= epsilon) return true;
    else return false;
  }
}

void SimObject::update(glm::vec4 rotationPoint)
{
  //Rotation Matrix
  glm::mat4 rotationMatrix = glm::rotate(
    glm::mat4(),
    mRadiansPerUpdate,
    glm::vec3(0.0f, 1.0f, 0.0f));
  //Get vector from the rotation point
  glm::vec4 vecFromRotation = mLocation - rotationPoint;
  //Rotate the vector from pivot to location
  vecFromRotation = rotationMatrix * vecFromRotation;
  //Add vecFromRotation to rotationPoint and set as new location
  mLocation = vecFromRotation + rotationPoint;
  mModelRotationMatrix = rotationMatrix * mModelRotationMatrix;
}

void SimObject::OrientToTarget(SimObject * target)
{
  glm::vec3 start = glm::normalize(mModelRotationMatrix[2]);
  glm::vec3 end = glm::normalize(target->mLocation - mLocation);
  if (colinear(start, end, .1))
  {
    return;
  }
  glm::quat roQuat = glm::rotation(start, end);
  mModelRotationMatrix = glm::mat4_cast(roQuat) * mModelRotationMatrix;
}

Ruber::Ruber()
  : SimObject("Models/Ruber.tri"
    , glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
    , 2000
    , 0.0f
    , 264 * 3) {}

Unum::Unum()
  : SimObject("Models/Unum.tri"
    , glm::vec4(4000.0f, 0.0f, 0.0f, 0.0f)
    , 200
    , 0.004f
    , 648 * 3)
{};

Duo::Duo()
  : SimObject("Models/Duo.tri"
    , glm::vec4(9000.0f, 0.0f, 0.0f, 0.0f)
    , 400
    , 0.002f
    , 264 * 3) {};

Primus::Primus()
  : SimObject("Models/Primus.tri"
    , glm::vec4(11000.0f, 0.0f, 0.0f, 0.0f)
    , 100
    , 0.002f
    , 264 * 3) {};

Secundus::Secundus()
  : SimObject("Models/Secundus.tri"
    , glm::vec4(13000.0f, 0.0f, 0.0f, 0.0f)
    , 150
    , 0.004f
    , 288 * 3)
{};
