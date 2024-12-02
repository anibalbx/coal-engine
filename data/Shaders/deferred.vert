#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

uniform vec3 position;
uniform vec4 u_color;
uniform mat4 u_lightSpaceMatrixDir[10];
uniform mat4 u_lightSpaceMatrixSpot[10];
uniform float u_numShadowLightsDir;
uniform float u_numShadowLightsSpot;
out vec3 normal;
out vec4 color;
out vec2 uv;
out vec4 FragLightPosDir[10];
out vec4 FragLightPosSpot[10];
out float num_shadows_dir;
out float num_shadows_spot;

void main()
{
  uv = a_uv;
  color = u_color;
  normal = a_normal;
  num_shadows_dir = u_numShadowLightsDir;
  num_shadows_spot = u_numShadowLightsSpot;

  for(int i = 0; i < u_numShadowLightsDir; i++){
    FragLightPosDir[i] = u_lightSpaceMatrixDir[i] * vec4(a_position, 1.0);
  }

  for(int i = 0; i < u_numShadowLightsSpot; i++){
    FragLightPosSpot[i] = u_lightSpaceMatrixSpot[i] * vec4(a_position, 1.0);
  }

  gl_Position = vec4(a_position,1.0);
}
