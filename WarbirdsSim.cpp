/*
Hongyou Xiong
COMP465 Warbirds Phase 2
*/
#define __Windows__
#include "includes465\include465.hpp"
#include "includes465\texture465.hpp"

# include "SimObject.h"
# include "WarBird.h"
# include "MissileBase.h"
# include <string>
# include <vector>
# include <algorithm>

///
/// Additional Declarations
///
enum TimeQuantumMode
{
  ACE,
  PILOT,
  TRAINEE,
  DEBUG
};

enum CameraMode
{
  FRONT,
  TOP,
  BIRD,
  UNUM,
  DUO
};

enum WarpLocation
{
  UNUM_WARP,
  DUO_WARP
};

///
/// Simulation Variables
///
int frameCount = 0;
double currentTime, lastTime, timeInterval;
const std::string vertexShaderFile{ "Shaders/simpleVertex.glsl" };
const std::string fragmentShaderFile{ "Shaders/simpleFragment.glsl" };
GLuint shaderProgram;

TimeQuantumMode timeQuantumMode = TimeQuantumMode::ACE;
CameraMode cameraMode = CameraMode::BIRD;
WarpLocation warp = WarpLocation::UNUM_WARP;
bool gravityOn = false;

///
/// Light Variables
///
bool spotLightOn = false;
bool headLightOn = false;
bool pointLightOn = false;
bool ambientLightOn = true;

///
/// Title Variables
///
std::string warBirdMissileCount = "?";
std::string unumMissileCount = "?";
std::string secundusMissileCount = "?";
std::string updateRate = "??";
std::string frameRate{ "?????" };
std::string cameraView = "*";

///
/// Simulated Objects Variables
///
const int numEntities = 7;
std::vector<SimObject*> entities;
Ruber* oRuber;
Unum* oUnum;
Duo* oDuo;
Primus* oPrimus;
Secundus* oSecundus;
WarBird* oWarBird;
UnumMissileSilo* oUnumMissileSilo;
SecundusMissileSilo* oSecundusMissileSilo;

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;

///
/// Skybox Variables
///
GLuint skyboxVAO; //skybox vertex array object
GLuint skyboxIBO; //skybox indices buffer object
GLuint skyboxVBO;
GLuint skyboxIsTexture;
struct SkyboxFace
{
  GLuint texture;
  glm::mat4 transformMatrix;
};
std::vector<SkyboxFace> faces;

/*Locations of the vertices*/
static const GLfloat planeVerts[16] = {
  -50000.0f, -50000.0f, 0.0f, 1.0f,	// Bottom Left Corner
  50000.0f, -50000.0f, 0.0f, 1.0f,	// Bottom Right Corner
  50000.0f, 50000.0f, 0.0f, 1.0f,		// Top Right Corner
  -50000.0f, 50000.0f, 0.0f, 1.0f		// Top Left Corner
};

/*The order the vertices will be drawn*/
static const unsigned int planeIndices[] = {
  0, 1, 2,
  2, 3, 0
};

/*The texture coordinates and order */
static const GLfloat texCoords[] = {
  0.0f, 0.0f, // Bottom Left
  1.0f, 0.0f, // Top Left
  1.0f, 1.0f, // Top Right
  0.0f, 1.0f, // Bottom Right
};

///
/// Shader Var locations
///
GLuint vNormalMatrix;
GLuint vModelViewProjection;
GLuint vShowTexture;

GLuint vHeadLightDirection;
GLuint vHeadLightIntensity;

GLuint vPointLightLocation;
GLuint vPointLightColor;

GLuint vSpotLightLocation;
GLuint vSpotLightDirection;
GLuint vSpotLightIntensity;

GLuint vAmbientLightOn;
GLuint vHeadLightOn;
GLuint vSpotLightOn;
GLuint vPointLightOn;

///-------------------------------------------------------------------------
/// Start of functions
///-------------------------------------------------------------------------
void SetupSkybox()
{
  glGenBuffers(1, &skyboxIBO); //Indices buffer object
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW); //Setup the indices in the shaders

  glGenVertexArrays(1, &skyboxVAO); //Vertex array object
  glBindVertexArray(skyboxVAO);

  glGenBuffers(1, &skyboxVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts) + sizeof(texCoords), NULL, GL_STATIC_DRAW); // Do not fill data yet
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(planeVerts), planeVerts); // Fill first 16 slots with vertex info
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(planeVerts), sizeof(texCoords), texCoords); // Fill last 8 with texture coordinate info.
  
  //Setup the passing of texture coordinates
  GLuint skyboxPosition = glGetAttribLocation(shaderProgram, "vPosition");
  glVertexAttribPointer(skyboxPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(skyboxPosition);

  //Setup the passing of texture coordinates
  GLuint skyboxTexCoord = glGetAttribLocation(shaderProgram, "vTexCoord");
  glVertexAttribPointer(skyboxTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(planeVerts)));
  glEnableVertexAttribArray(skyboxTexCoord);
}

void SetupSkyboxFaces()
{
  std::vector<glm::mat4> faceTransforms;
  faceTransforms.push_back(glm::translate(glm::mat4(), glm::vec3(0, 0, 50000.0f))); // Front
  faceTransforms.push_back(glm::translate(glm::mat4(), glm::vec3(0, 0, -50000.0f))); // Back
  faceTransforms.push_back(glm::translate(glm::mat4(), glm::vec3(0, 50000.0f, 0)) * glm::rotate(PI / 2, glm::vec3(1.0f, 0, 0))); // Top
  faceTransforms.push_back(glm::translate(glm::mat4(), glm::vec3(0, -50000.0f, 0)) * glm::rotate(3 * PI / 2, glm::vec3(1.0f, 0, 0))); // Bottom
  faceTransforms.push_back(glm::translate(glm::mat4(), glm::vec3(-50000.0f, 0, 0)) * glm::rotate(PI / 2, glm::vec3(0, 1.0f, 0))); // Left
  faceTransforms.push_back(glm::translate(glm::mat4(), glm::vec3(50000.0f, 0, 0)) * glm::rotate(3 * PI / 2, glm::vec3(0, 1.0f, 0))); // Right

  GLuint texData = 0;
  for (int i = 0; i < 6; i++)
  {
    SkyboxFace skyboxFace;

    std::string fileName{ "SkyboxImages/Face" };
    fileName += '0' + (i+1);
    fileName += ".raw";

    texData = loadRawTexture(texData, fileName.c_str(), 512, 512);

    skyboxFace.transformMatrix = faceTransforms[i];
    skyboxFace.texture = texData;

    faces.push_back(skyboxFace);
  }
}

void InitShaders()
{
  shaderProgram = loadShaders((char*)vertexShaderFile.c_str(), (char*)fragmentShaderFile.c_str());
  glUseProgram(shaderProgram);
}

void InitSimObjects()
{
  oRuber = new Ruber();
  oUnum = new Unum();
  oDuo = new Duo();
  oPrimus = new Primus();
  oSecundus = new Secundus();
  oWarBird = new WarBird();
  oUnumMissileSilo = new UnumMissileSilo();
  oSecundusMissileSilo = new SecundusMissileSilo();

  entities.push_back(oRuber);
  entities.push_back(oUnum);
  entities.push_back(oDuo);
  entities.push_back(oPrimus);
  entities.push_back(oSecundus);
  entities.push_back(oWarBird);
  entities.push_back(oUnumMissileSilo);
  entities.push_back(oSecundusMissileSilo);
}

void InitSkybox()
{
  SetupSkybox();
  SetupSkyboxFaces();
}

void SetupLighting()
{
  glUniform1ui(vAmbientLightOn, ambientLightOn);
  glUniform1ui(vHeadLightOn, headLightOn);
  glUniform1ui(vSpotLightOn, spotLightOn);
  glUniform1ui(vPointLightOn, pointLightOn);
}

void InitShaderVars()
{
  vAmbientLightOn = glGetUniformLocation(shaderProgram, "AmbientLightOn");
  vHeadLightOn = glGetUniformLocation(shaderProgram, "HeadLightOn");
  vSpotLightOn = glGetUniformLocation(shaderProgram, "SpotLightOn");
  vPointLightOn = glGetUniformLocation(shaderProgram, "PointLightOn");

  vNormalMatrix = glGetUniformLocation(shaderProgram, "NormalMatrix");
  vModelViewProjection = glGetUniformLocation(shaderProgram, "MVP");
  vShowTexture = glGetUniformLocation(shaderProgram, "IsTexture");

  vHeadLightDirection = glGetUniformLocation(shaderProgram, "HeadLightDirection");
  vHeadLightIntensity = glGetUniformLocation(shaderProgram, "HeadLightIntensity");

  vPointLightLocation = glGetUniformLocation(shaderProgram, "PointLightLocation");
  vPointLightColor = glGetUniformLocation(shaderProgram, "PointLightColor");

  vSpotLightLocation = glGetUniformLocation(shaderProgram, "SpotLightPosition");
  vSpotLightDirection = glGetUniformLocation(shaderProgram, "SpotLightDirection");
  vSpotLightIntensity = glGetUniformLocation(shaderProgram, "SpotLightIntensity");
}

void GenerateBuffer(SimObject* obj)
{
  GLuint vao[1];
  glGenVertexArrays(1, vao);
  obj->mVao = vao[0];

  GLuint buffer[1];
  glGenBuffers(1, buffer);
  obj->mBuffer = buffer[0];

  obj->mBoundingRadius = loadModelBuffer(
    (char*)obj->mModelName.c_str(),
    obj->mVertexCount, obj->mVao, obj->mBuffer, shaderProgram,
    obj->vPosition, obj->vColor, obj->vNormal, "vPosition", "vColor", "vNormal");
}

void CreateWarBirdMissile()
{
  if (!oWarBird->IsDead() && !oWarBird->HasMissile())
  {
    WarBirdMissile* warBirdMissile = new WarBirdMissile(oWarBird->mLocation);
    oWarBird->SetMissile(warBirdMissile);
    entities.push_back(warBirdMissile);
    GenerateBuffer(warBirdMissile);

    warBirdMissile->mModelRotationMatrix = oWarBird->mModelRotationMatrix;
  }
}

void CreateUnumBaseMissile()
{
  if (!oUnumMissileSilo->IsDead() && !oUnumMissileSilo->HasMissile() && oUnumMissileSilo->IsReady())
  {
    BaseMissile* unumBaseMissile = new BaseMissile(oUnumMissileSilo->mLocation);
    oUnumMissileSilo->SetMissile(unumBaseMissile);
    entities.push_back(unumBaseMissile);
    GenerateBuffer(unumBaseMissile);

    unumBaseMissile->OrientToTarget(oWarBird);
  }
}

void CreateSecundusBaseMissile()
{
  if (!oSecundusMissileSilo->IsDead() && !oSecundusMissileSilo->HasMissile() && oUnumMissileSilo->IsReady())
  {
    BaseMissile* secundusBaseMissile = new BaseMissile(oSecundusMissileSilo->mLocation);
    oSecundusMissileSilo->SetMissile(secundusBaseMissile);
    entities.push_back(secundusBaseMissile);
    GenerateBuffer(secundusBaseMissile);

    secundusBaseMissile->OrientToTarget(oWarBird);
  }
}

void selectCamera()
{
  glm::vec3 eye;
  glm::vec3 looking;
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  switch (cameraMode)
  {
    default:
    case CameraMode::FRONT: // FRONT CAMERA
      eye = glm::vec3(0.0f, 10000.0f, 20000.0f);
      looking = glm::vec3(0);
      cameraView = "Front";
      break;
    case CameraMode::TOP: // TOP CAMERA
      eye = glm::vec3(0.0f, 20000.0f, 0.0f);
      looking = glm::vec3(0);
      up = glm::vec3(0.0f, 0.0f, -1.0f);
      cameraView = "Top";
      break;
    case CameraMode::BIRD: // BIRD CAMERA
      eye = oWarBird->mLocation + oWarBird->mModelRotationMatrix * glm::vec4(0.0f, 300.0f, -1000.0f, 0.0f);
      looking = oWarBird->mLocation + oWarBird->mModelRotationMatrix * glm::vec4(0.0f, 300.0f, 0.0f, 0.0f);
      cameraView = "Warbird";
      break;
    case CameraMode::UNUM: // UNUM CAMERA
      //mModelRotationMatrix[2] ==> "at" vector
      eye = oUnum->mLocation + oUnum->mModelRotationMatrix * glm::vec4(-4000.0f, 0.0f, -4000.0f, 0.0f);
      looking = oUnum->mLocation;
      cameraView = "Unum";
      break;
    case CameraMode::DUO: // DUO CAMERA
      //mModelRotationMatrix[2] ==> "at" vector
      eye = oDuo->mLocation + oDuo->mModelRotationMatrix * glm::vec4(-4000.0f, 0.0f, -4000.0f, 0.0f);
      looking = oDuo->mLocation;
      cameraView = "Duo";
      break;
  }
  viewMatrix = glm::lookAt(eye, looking, up);
}

uint32_t getTimeQuantum()
{
  switch (timeQuantumMode)
  {
    default:
    case TimeQuantumMode::ACE:
      return 5;
    case TimeQuantumMode::PILOT:
      return 40;
    case TimeQuantumMode::TRAINEE:
      return 100;
    case TimeQuantumMode::DEBUG:
      return 500;
  }
}

void updateTitleVariables()
{
  // UpdateRate
  int updatesPerSecond = (int)(1000.0f / getTimeQuantum());
  updateRate = std::to_string(updatesPerSecond);
  
  // Missiles
  warBirdMissileCount = std::to_string(oWarBird->mMissileCount);
  unumMissileCount = std::to_string(oUnumMissileSilo->mMissileCount);
  secundusMissileCount = std::to_string(oSecundusMissileSilo->mMissileCount);
}

void updateTitleText()
{
  char title[1000];
  if (oUnumMissileSilo->IsDead() && oSecundusMissileSilo->IsDead())
  {
    std::snprintf(title, 1000, "Cadet passes flight training");
  }
  else if (oWarBird->IsDead())
  {
    std::snprintf(title, 1000, "Cadet resigns from War College");
  }
  else
  {
    std::snprintf(title, 1000
      , "WarBird %s Unum %s Secundus %s U/S %2s F/S %5s View %s"
      , (char*)warBirdMissileCount.c_str()
      , (char*)unumMissileCount.c_str()
      , (char*)secundusMissileCount.c_str()
      , (char*)updateRate.c_str()
      , (char*)frameRate.c_str()
      , (char*)cameraView.c_str());
  }
  glutSetWindowTitle(title);
}

void ToggleAmbientLight() {
  ambientLightOn = !ambientLightOn;
  GLuint vAmbientLightOn = glGetUniformLocation(shaderProgram, "AmbientLightOn");
  glUniform1ui(vAmbientLightOn, ambientLightOn);
}

void ToggleSpotLight() {
  spotLightOn = !spotLightOn;
  GLuint vSpotLightOn = glGetUniformLocation(shaderProgram, "SpotLightOn");
  glUniform1ui(vSpotLightOn, spotLightOn);
}

void ToggleHeadLight() {
  headLightOn = !headLightOn;
  GLuint vHeadLightOn = glGetUniformLocation(shaderProgram, "HeadLightOn");
  glUniform1ui(vHeadLightOn, headLightOn);
}

void TogglePointLight() {
  pointLightOn = !pointLightOn;
  GLuint vPointLightOn = glGetUniformLocation(shaderProgram, "PointLightOn");
  glUniform1ui(vPointLightOn, pointLightOn);
}

void SetLights()
{
  if (headLightOn)
  {
    glUniform3f(vHeadLightDirection, 0.0, 0.0, 1.0f);
  }
  if (pointLightOn)
  {
    glUniform3f(vPointLightLocation, 0.0f, 0.0f, 0.0f); // Ruber
  }
  if (spotLightOn)
  {
    glm::vec4 eye = oWarBird->mLocation + oWarBird->mModelRotationMatrix * glm::vec4(0.0f, 300.0f, -1000.0f, 0.0f);
    glm::vec4 looking = oWarBird->mLocation + oWarBird->mModelRotationMatrix * glm::vec4(0.0f, 300.0f, 0.0f, 0.0f);

    looking = looking - eye;

    glm::vec4 normalizedLooking = glm::normalize(looking);
    glUniform3f(vSpotLightLocation, eye.x, eye.y, eye.z);
    glUniform3f(vSpotLightDirection, looking.x, looking.y, looking.z);
    glUniform1f(vSpotLightIntensity, 2.0f);
  }
}

void DrawSkybox()
{
  glUniform1f(vShowTexture, 1);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxIBO);
  for (int i = 0; i < 6; i++)
  {
    glBindTexture(GL_TEXTURE_2D, faces[i].texture);
    glm::mat4 modelViewMatrix = viewMatrix * faces[i].transformMatrix;
    glm::mat3 normalMatrix = glm::mat3(modelViewMatrix);
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    glUniformMatrix3fv(vNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(vModelViewProjection, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
  }
}

void DrawObjects()
{
  glUniform1f(vShowTexture, 0);
  for (SimObject* obj : entities)
  {
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(), glm::vec3(obj->mRadius * 1.0f / obj->mBoundingRadius));
    glm::mat4 modelMatrix = glm::translate(glm::mat4(), glm::vec3(obj->mLocation)) * obj->mModelRotationMatrix * scaleMatrix;
    glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
    glm::mat3 normalMatrix = glm::mat3(modelViewMatrix);
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    glUniformMatrix3fv(vNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniformMatrix4fv(vModelViewProjection, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
    glBindVertexArray(obj->mVao);
    if (obj->mModelName == "Models/Ruber.tri")
    {
      GLuint isRuber = glGetUniformLocation(shaderProgram, "Ruber");
      glUniform1ui(isRuber, true);
      glDrawArrays(GL_TRIANGLES, 0, obj->mVertexCount);
      glUniform1ui(isRuber, false);
      continue;
    }
    glDrawArrays(GL_TRIANGLES, 0, obj->mVertexCount);
  }
}

void display(void) {
  selectCamera();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  SetLights();
  DrawObjects();
  DrawSkybox();

	glutSwapBuffers();
  
  //Update frame count
  frameCount++;
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  timeInterval = currentTime - lastTime;
  if (timeInterval >= 1000)
  {
    lastTime = currentTime;
    frameRate = std::to_string(frameCount);
    frameCount = 0;
  }
}

void init(void) {
  InitSimObjects();
  InitShaders();
  InitSkybox();
  InitShaderVars();
  SetupLighting();

  for (SimObject* obj : entities)
  {
    GenerateBuffer(obj);
  }

  // set render state values
	glEnable(GL_DEPTH_TEST);
  //Setting clear color to black. Space isn't grey
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  lastTime = glutGet(GLUT_ELAPSED_TIME);
}

// set viewport and projectionMatrix on window resize events
void reshape(int width, int height) {
	glViewport(0, 0, width, height);
  GLfloat aspectRatio = (GLfloat) width / (GLfloat)height;
	// set projection matrix
  projectionMatrix = glm::perspective(glm::radians(60.0f), aspectRatio, 1.0f, 100000.0f);
	}

void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
  //Camera switch : v to go next, x to go previous
  //Q to quit.
  glm::vec4 location;
  glm::vec3 targetVector;
  glm::vec3 axisRotation;
  glm::vec3 currentDirection;
  glm::mat4 applyThisRotation;

  glm::mat4 directionalMatrix;

  switch (key)
  {
  case 'a': case 'A': // Toggle Ambient Light
    ToggleAmbientLight();
    break;
  case 'f': case 'F': // Launch available ship missile
    CreateWarBirdMissile();
    break;
  case 'g': case 'G': // Toggle gravity towards Ruber. No other planets have gravity for simplicity
    gravityOn = true;
    break;
  case 'h': case 'H': // Toggle Head Light
    ToggleHeadLight();
    break;
  case 'l': case 'L': // Toggle Spot Light
    ToggleSpotLight();
    break;
  case 'p': case 'P': // Toggle Point Light
    TogglePointLight();
    break;
  case 'q': case 'Q': // Quit Game
    exit(EXIT_SUCCESS);
    break;
  case 's': case 'S': // Select next speed on ship
    oWarBird->UseNextThruster();
    break;
  case 't': case 'T': // Set time quantum
    timeQuantumMode = static_cast<TimeQuantumMode>((timeQuantumMode + 1) % 4);
    break;
  case 'v': case 'V': // Select next camera
    cameraMode = static_cast<CameraMode>((5 + cameraMode + 1) % 5);
    break;
  case 'w': case 'W': // Warp!
    switch (warp)
    {
    case WarpLocation::UNUM_WARP:
      oWarBird->mLocation = oUnum->mLocation + oUnum->mModelRotationMatrix * glm::vec4(-4000.0f, 0.0f, -4000.0f, 0.0f);
      oWarBird->OrientToTarget(oUnum);
      break;
    case WarpLocation::DUO_WARP:
      oWarBird->mLocation = oDuo->mLocation + oDuo->mModelRotationMatrix * glm::vec4(-4000.0f, 0.0f, -4000.0f, 0.0f);
      oWarBird->OrientToTarget(oDuo);
      break;
    }
    warp = static_cast<WarpLocation>((warp + 1) % 2);
    break;
  case 'x': case 'X': // Select previous camera
    cameraMode = static_cast<CameraMode>((5 + cameraMode - 1) % 5);
    break;
  }
}

void specialKeyboard(int key, int /*x*/, int /*y*/)
{
  bool ctrlActive = glutGetModifiers() == GLUT_ACTIVE_CTRL;
  oWarBird->SetKeysPressed(key, ctrlActive);
}

void specialKeyboardUp(int key, int /*x*/, int /*y*/)
{
  bool ctrlActive = glutGetModifiers() == GLUT_ACTIVE_CTRL;
  oWarBird->SetKeysReleased(key, ctrlActive);
}

void update(void) {
  oUnum->update(oRuber->mLocation);

  // Keep track of distance planet moved as well.
  glm::vec4 oldDuoLocation = oDuo->mLocation;
  oDuo->update(oRuber->mLocation);
  glm::vec4 duoDelta = oDuo->mLocation - oldDuoLocation;
  
  // After applying the rotation, also move moons same distance that planet traveled
  oPrimus->update(oDuo->mLocation);
  oPrimus->mLocation += duoDelta;
  oSecundus->update(oDuo->mLocation);
  oSecundus->mLocation += duoDelta;

  // Attach the missile Silos to the planets
  oUnumMissileSilo->mLocation = oUnum->mLocation + glm::vec4(0.0f, oUnum->mRadius, 0.0f, 0.0f);
  oUnumMissileSilo->OrientToTarget(oWarBird);
  oUnumMissileSilo->UpdateState(entities);
  if (oUnumMissileSilo->FoundShip(oWarBird))
  {
    CreateUnumBaseMissile();
  }

  if (oUnumMissileSilo->HasMissile())
  {
    if (oUnumMissileSilo->IsDead() || oUnumMissileSilo->mMissile->GetMissileState() == Missile::MISSILE_STATE::DEAD)
    {
      Missile* missile = oUnumMissileSilo->mMissile;

      //Decouple missile from WarBird
      oUnumMissileSilo->mMissile = nullptr;
      //Remove object from entities list
      entities.erase(std::remove(entities.begin(), entities.end(), missile), entities.end());
      //Delete the missile
      delete missile;
    }
    else
    {
      oUnumMissileSilo->mMissile->UpdateState(entities);
    }
  }

  oSecundusMissileSilo->mLocation = oSecundus->mLocation + glm::vec4(0.0f, oSecundus->mRadius, 0.0f, 0.0f);
  oSecundusMissileSilo->OrientToTarget(oWarBird);
  oSecundusMissileSilo->UpdateState(entities);
  if (oSecundusMissileSilo->FoundShip(oWarBird))
  {
    CreateSecundusBaseMissile();
  }

  if (oSecundusMissileSilo->HasMissile())
  {
    if (oSecundusMissileSilo->IsDead() || oSecundusMissileSilo->mMissile->GetMissileState() == Missile::MISSILE_STATE::DEAD)
    {
      Missile* missile = oSecundusMissileSilo->mMissile;
      //Decouple missile from WarBird
      oSecundusMissileSilo->mMissile = nullptr;
      //Remove object from entities list
      entities.erase(std::remove(entities.begin(), entities.end(), missile), entities.end());
      //Delete the missile
      delete missile;
    }
    else
    {
      oSecundusMissileSilo->mMissile->UpdateState(entities);
    }
  }

  oWarBird->UpdateMovement(gravityOn);
  oWarBird->CheckForCollisions(entities);
  if (oWarBird->HasMissile())
  {
    if (oWarBird->IsDead() || oWarBird->mMissile->GetMissileState() == Missile::MISSILE_STATE::DEAD)
    {
      Missile* missile = oWarBird->mMissile;

      //Decouple missile from WarBird
      oWarBird->mMissile = nullptr;
      //Remove object from entities list
      entities.erase(std::remove(entities.begin(), entities.end(), missile), entities.end());
      //Delete the missile
      delete missile;
    }
    else
    {
      oWarBird->mMissile->UpdateState(entities);
    }
  }
}

void idleAlways(void) {
  updateTitleVariables(); // Update title bar variables
  updateTitleText(); // Always update the title
  glutPostRedisplay(); // Always draw
}

// Estimate FPS, use for fixed interval timer driven animation
void intervalTimer(int i) {
	glutTimerFunc(getTimeQuantum(), intervalTimer, 1); // reset timerDelay
  update();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	// Uncomment the following line to force OpenGL & GLSL 3.3
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("WarBird Simulation Phase 1");
	// initialize and verify glew
	glewExperimental = GL_TRUE;  // needed my home system 
	GLenum err = glewInit();
	if (GLEW_OK != err)
		printf("GLEW Error: %s \n", glewGetErrorString(err));
	else {
		printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
		printf("OpenGL %s, GLSL %s\n",
			glGetString(GL_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	// initialize scene here
	init();

  // set glut callback functions here
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeyboard);
  glutSpecialUpFunc(specialKeyboardUp);

  glutIdleFunc(idleAlways);
  glutTimerFunc(getTimeQuantum(), intervalTimer, 1); //Update at timeQuantumSpeed.
	glutMainLoop();
	printf("done\n");
	return 0;
}