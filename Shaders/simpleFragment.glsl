# version 330 core

in vec2 vs_texCoord;
in vec3 vs_normal, vs_worldPos;
in vec4 vs_color;

uniform bool AmbientLightOn;

uniform bool HeadLightOn;
uniform vec3 HeadLightDirection;

uniform bool Ruber;
uniform bool PointLightOn;
uniform vec3 PointLightPosition;

uniform bool SpotLightOn;
uniform vec3 SpotLightPosition;
uniform vec3 SpotLightDirection;
uniform float SpotLightIntensity;
float cutoffAngle = radians(5.0); //5 degrees

uniform bool IsTexture;
uniform sampler2D Texture;

//Debug options
bool DebugOn = true;
vec3 ambientColor = vec3(1.0, 0.0, 0.0);
vec3 diffuseColor = vec3(0.0, 1.0, 0.0);

out vec4 color;

vec4 addHeadLight()
{
  float ambient = 0.0f;
  if (AmbientLightOn)
  {
    ambient = 0.2f;
  }

  vec3 nHeadDir = normalize(HeadLightDirection);

  float diffuse = max(dot(vs_normal, nHeadDir), 0.0);
  return (ambient + diffuse) * vs_color;
}

vec4 addPointLight()
{
  float ambient = 0.0f;
  if (AmbientLightOn)
  {
    ambient = 0.2f;
  }

  vec3 lightDirection = normalize(PointLightPosition - vs_worldPos);
  float diffuse = 0.0;
  if (Ruber)
  {
    diffuse = max(dot(-vs_normal, lightDirection), 0.0);
  }
  else
  {
    diffuse = max(dot(vs_normal, lightDirection), 0.0);
  }

  return (ambient + diffuse) * vs_color;
}

vec4 addSpotLight()
{
  vec3 surfaceToLight = normalize(SpotLightPosition - vs_worldPos);
  vec3 lightDirection = normalize(SpotLightDirection);

  float angleFromCenter = acos(dot(-surfaceToLight, lightDirection));

  if (angleFromCenter < cutoffAngle)
  {
	return vec4(1, 0, 0, 1);
  }
  return vec4(angleFromCenter);


  float diffuse = max(dot(surfaceToLight, vs_normal), 0.0);
}

void main(void) {
  float ambientLight = 0.0;
  if (AmbientLightOn)
  {
    ambientLight = 0.1;
  }
  vec4 tempColor = vs_color * ambientLight;

  if (HeadLightOn) tempColor += addHeadLight();

  if (PointLightOn)
  {
    tempColor += addPointLight();
  }

  if (SpotLightOn)
  {
    tempColor += addSpotLight();
  }

  if (IsTexture) // use texture on surface
    color = texture(Texture, vs_texCoord);
  else // use color for surface
    color = tempColor;
}