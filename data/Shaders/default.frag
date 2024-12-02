#version 330
out vec4 fragColor;
in vec3 normal;
in vec2 uv;
uniform sampler2D my_texture;
const float offset = 1.0 / 300.0;

void main()
{
  fragColor = vec4(texture(my_texture, uv).rgb,1.0); 
}