#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

uniform vec3 position;
uniform mat4 u_mvp;
uniform vec4 u_color;
uniform mat4 u_model;
uniform mat4 u_lightSpaceMatrix;
out vec3 normal;
out vec4 color;
out vec2 uv;
out vec3 WorldPos;
out vec4 FragLightPos;

void main()
{
  uv = a_uv;
  color = u_color;
  WorldPos = vec3(u_model * vec4(a_position, 1.0));
  normal = mat3(u_model) * a_normal;
  FragLightPos = u_lightSpaceMatrix * vec4(WorldPos, 1.0);
  gl_Position = u_mvp * vec4(a_position,1.0);
}
