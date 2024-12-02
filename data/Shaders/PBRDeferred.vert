#version 330
      layout (location = 0) in vec3 a_position;
      layout (location = 1) in vec3 a_normal;
      layout (location = 2) in vec2 a_uv;

      //uniform mat4 u_lightSpaceMatrixDir[10];
      //uniform mat4 u_lightSpaceMatrixSpot[10];
      uniform sampler2D u_texture_position;

      out vec2 uv;
      out vec3 WorldPos;

      //out vec4 FragLightPosDir[10];
      //out vec4 FragLightPosSpot[10];


      void main()
      {
         gl_Position =  vec4(a_position,1.0);
         uv = a_uv;
         vec3 WorldPos = texture(u_texture_position,uv).rgb;
      }
