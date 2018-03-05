# version 330 core

in vec2 vTexCoord;
in vec3 vNormal;
in vec4 vPosition;
in vec4 vColor;

uniform bool IsTexture;

uniform mat3 NormalMatrix;
uniform mat4 MVP;  // = projection * view * model

out vec2 vs_texCoord;
out vec3 vs_normal, vs_worldPos;
out vec4 vs_color;

void main(void) {
  vec4 position = MVP * vPosition;
  gl_Position = position;
  vs_worldPos = position.xyz;
  vs_normal = normalize(NormalMatrix * vNormal);

  if (IsTexture)
  {
    vs_texCoord = vTexCoord;
	vs_color = vec4(0, 0, 0, 0);
  }
  else
  {
	vs_texCoord = vec2(0, 0);
	vs_color = vColor;
  }
}