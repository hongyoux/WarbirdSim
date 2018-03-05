#pragma once

# include <GL/glew.h>
# include <GL/freeglut.h>
# include <glm/glm.hpp>
# include <glm/gtc/quaternion.hpp>
# include <glm/gtx/quaternion.hpp>
# include <glm/gtc/constants.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtx/rotate_vector.hpp>
# include <string>

class SimObject
{
public:
  SimObject(const std::string modelName, const glm::vec4 location, float radius, float radiansPerUpdate, uint32_t vertCount)
    : mModelName(modelName)
    , mLocation(std::move(location))
    , mRadius(radius)
    , mRadiansPerUpdate(radiansPerUpdate)
    , mVertexCount(vertCount)
    , mMissileCount(0)
    {};

  virtual void update(glm::vec4 rotationPoint);

  void SimObject::OrientToTarget(SimObject * target);

  const std::string mModelName;
	glm::vec4 mLocation;
	const float mRadius;
  const float mRadiansPerUpdate;
  const uint32_t mVertexCount;
  float mBoundingRadius;
  uint32_t mMissileCount;
  glm::mat4 mModelRotationMatrix;
  
  //OpenGL related Variables
  GLuint mVao;
  GLuint mBuffer;
  GLuint vPosition;
  GLuint vColor;
  GLuint vNormal;

};

class Ruber : public SimObject
{
public:
  Ruber();
};

class Unum : public SimObject
{
public:
  Unum();
};

class Duo : public SimObject
{
public:
  Duo();
};

class Primus : public SimObject
{
public:
  Primus();
};

class Secundus : public SimObject
{
public:
  Secundus();
};
