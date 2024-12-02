 #version 330
 layout (location = 0) in vec3 a_position;
 layout (location = 1) in vec3 a_normal;
 layout (location = 2) in vec2 a_uv;
 out vec3 normal;
 out vec2 uv;
 void main()
 {
 gl_Position =  vec4(a_position,1.0);
 normal = a_normal;
 uv = a_uv;
 }