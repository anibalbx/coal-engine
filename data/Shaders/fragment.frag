 #version 330
      out vec4 fragColor;
      in vec3 normal;
      in vec2 uv;
      uniform sampler2D my_texture;
      const float offset = 1.0 / 300.0;

      void main()
      {

      // Basic Frame Buffer
      //fragColor = vec4(texture(my_texture, uv).rgb,1.0); 
      //fragColor = vec4(1.0,0.0,0.0,1.0);


      // Inverse Color Post-Process
      //fragColor = vec4(vec3(1.0 - texture(my_texture, uv)), 1.0); 


      // GrayScale
      //fragColor = texture(my_texture, uv);
      //float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;  
      //fragColor = vec4(average, average, average, 1.0);



    // Kernel
    //---------------------------------------------------------------------------------------------------------------------------------------------------------

      vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );


     
    /*float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );*/

     // EDGE DETECTION
     /*float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );*/


    // BLUR
    float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(my_texture, uv.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    fragColor = vec4(col, 1.0);
      }