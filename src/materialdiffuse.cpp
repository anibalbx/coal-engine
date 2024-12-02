#include "materialdiffuse.h"

#include "RenderList.h"
coalengine::MaterialDiffuse::MaterialDiffuse()
{
  mat_type_ = coalengine::Material::Type::kMaterial_Diffuse;

  vertex_source_ = R"(
      #version 330
      layout (location = 0) in vec3 a_position;
      layout (location = 1) in vec3 a_normal;
      layout (location = 2) in vec2 a_uv;
      uniform vec3 position;
      uniform mat4 u_model;
      uniform mat4 u_mvp;
      uniform vec4 u_color;
      out vec3 normal;
      out vec3 FragPos;
      out vec4 color;
      out vec2 uv;
      void main()
      {
      gl_Position = u_mvp * vec4(a_position,1.0);
      color = u_color;
      FragPos = vec3(u_model * vec4(a_position, 1.0));
      normal = a_normal;
      uv = a_uv;
      }
  )";

  fragment_source_ = R"(
    #version 330
    layout (location = 0) out vec4 fragColor;
    layout (location = 1) out vec4 BrightColor;
    in vec3 normal;
    in vec4 color;
    in vec2 uv;
    in vec3 FragPos;

    uniform float time;
    uniform sampler2D my_texture;
    uniform bool u_emissive;
    uniform bool u_activebloom;


    uniform vec3 u_LightDirPosition[10];
    uniform vec3 u_LightDirDirection[10];
    uniform vec3 u_LightDirColor[10];
    uniform float u_NumDirectionals;



    uniform vec3 u_LightDirPositionShadows[10];
    uniform vec3 u_LightDirDirectionShadows[10];
    uniform vec3 u_LightDirColorShadows[10];
    uniform float u_NumDirectionalsShadow;



    uniform vec3 u_LightSpotPosition[10];
    uniform vec3 u_LightSpotDirection[10];
    uniform vec3 u_LightSpotColor[10];
    uniform float u_LightSpotInnerAngle[10];
    uniform float u_LightSpotOuterAngle[10];
    uniform float u_NumSpots;


    uniform vec3 u_LightSpotPositionShadows[10];
    uniform vec3 u_LightSpotDirectionShadows[10];
    uniform vec3 u_LightSpotColorShadows[10];
    uniform float u_LightSpotInnerAngleShadows[10];
    uniform float u_LightSpotOuterAngleShadows[10];
    uniform float u_NumSpotsShadow;

    uniform vec3 u_LightPointPosition[10];
    uniform vec3 u_LightPointDirection[10];
    uniform vec3 u_LightPointColor[10];
    uniform float u_NumPoints;

    void main()
    {
      vec3 lighting = vec3(0.0,0.0,0.0);

      for(int i = 0; i < u_NumDirectionals; ++i)
          {
             float ambientStrength = 0.1;
             vec3 ambient = ambientStrength * u_LightDirColor[i];
  	
             // diffuse 
             vec3 norm = normalize(normal);
             vec3 lightDir = normalize(-u_LightDirDirection[i]);
             float diff = max(dot(norm, lightDir), 0.0);
             vec3 diffuse = diff * u_LightDirColor[i];
                     
             lighting += (ambient + diffuse) * color.rbg;
           }


     for(int i = 0; i < u_NumDirectionalsShadow; ++i)
          {
             float ambientStrength = 0.1;
             vec3 ambient = ambientStrength * u_LightDirColorShadows[i];
  	
             // diffuse 
             vec3 norm = normalize(normal);
             vec3 lightDir = normalize(-u_LightDirDirectionShadows[i]);
             float diff = max(dot(norm, lightDir), 0.0);
             vec3 diffuse = diff * u_LightDirColorShadows[i];
                     
             lighting += (ambient + diffuse) * color.rbg;
           }


      for(int i = 0; i < u_NumSpots; ++i)
          {
             float distance = length(u_LightSpotPosition[i]- FragPos);
             float ambientStrength = 0.1;
             vec3 ambient = ambientStrength * u_LightSpotColor[i];
  	
             // diffuse 
             vec3 norm = normalize(normal);
             vec3 lightDir = normalize(u_LightSpotPosition[i] - FragPos);
             float diff = max(dot(norm, lightDir), 0.0);
             vec3 diffuse = diff * u_LightSpotColor[i];

             float attenuation = 1.0 / (distance * distance);
             diffuse *= attenuation;

             float theta     = dot(lightDir, normalize(-u_LightSpotDirection[i]));
             float epsilon   = u_LightSpotInnerAngle[i] - u_LightSpotOuterAngle[i];
             float intensity = clamp((theta - u_LightSpotOuterAngle[i]) / epsilon, 0.0, 1.0);
                     
             lighting += ((ambient + diffuse) * color.rbg) * intensity;
           }


    for(int i = 0; i < u_NumSpotsShadow; ++i)
          {
             float distance = length(u_LightSpotPositionShadows[i]- FragPos);
             float ambientStrength = 0.1;
             vec3 ambient = ambientStrength * u_LightSpotColorShadows[i];
  	
             // diffuse 
             vec3 norm = normalize(normal);
             vec3 lightDir = normalize(u_LightSpotPositionShadows[i] - FragPos);
             float diff = max(dot(norm, lightDir), 0.0);
             vec3 diffuse = diff * u_LightSpotColorShadows[i];

             float attenuation = 1.0 / (distance * distance);
             diffuse *= attenuation;

             float theta     = dot(lightDir, normalize(-u_LightSpotDirectionShadows[i]));
             float epsilon   = u_LightSpotInnerAngleShadows[i] - u_LightSpotOuterAngleShadows[i];
             float intensity = clamp((theta - u_LightSpotOuterAngleShadows[i]) / epsilon, 0.0, 1.0);
                     
             lighting += ((ambient + diffuse) * color.rbg) * intensity;
           }

       
      for(int i = 0; i < u_NumPoints; ++i)
          {
             float distance = length(u_LightPointPosition[i]- FragPos);
             float ambientStrength = 0.1;
             vec3 ambient = ambientStrength * u_LightPointColor[i];
  	
             // diffuse 
             vec3 norm = normalize(normal);
             vec3 lightDir = normalize(u_LightPointPosition[i] - FragPos);
             float diff = max(dot(norm, lightDir), 0.0);
             vec3 diffuse = diff * u_LightPointColor[i];

             float attenuation = 1.0 / (distance * distance);
             diffuse *= attenuation;
                          
             lighting += ((ambient + diffuse) * color.rbg);
           }


      vec4 final_color = color;

      if(u_emissive)
      {
        final_color *= 100.0;
        fragColor = vec4(texture(my_texture, uv).rgb,1.0) + final_color;
      }

      if(!u_activebloom)
      {
        // HDR tonemapping
        lighting = lighting / (lighting + vec3(1.0));

        vec3 mapped = vec3(1.0) - exp(-lighting * 2.0);
        // gamma correct
        lighting = pow(mapped, vec3(1.0/2.2));      
      }

      if(!u_emissive)
      {
        fragColor = vec4(texture(my_texture, uv).rgb,1.0) + vec4(lighting,1.0);
      }

      float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
      if(brightness > 1.0)
        BrightColor = vec4(fragColor.rgb, 1.0);
      else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    /*void main()
    {
    	float intensity;
    	vec4 color;

    	intensity = dot(-u_LightDirDirectionShadows[0],normalize(normal));
    
    	if (intensity > 0.95)
    		color = vec4(1.0,0.5,0.5,1.0);
    	else if (intensity > 0.5)
    		color = vec4(0.6,0.3,0.3,1.0);
    	else if (intensity > 0.25)
    		color = vec4(0.4,0.2,0.2,1.0);
    	else
    		color = vec4(0.2,0.1,0.1,1.0);
    	gl_FragColor = color;
    
    }*/
  )";

  is_emissive_ = false;
}


coalengine::MaterialDiffuse::MaterialDiffuse(MaterialDiffuse&& other) : coalengine::Material::Material(std::move(other))
{
  is_emissive_ = other.is_emissive_;
}

coalengine::MaterialDiffuse::~MaterialDiffuse()
{
}

coalengine::MaterialDiffuse& coalengine::MaterialDiffuse::operator=(MaterialDiffuse&& other)
{
  Material::operator=(std::move(other));
  return *this;
}

void coalengine::MaterialDiffuse::SetIsEmissive(bool value)
{
  is_emissive_ = value;
}

bool coalengine::MaterialDiffuse::GetIsEmissive()
{
  return is_emissive_;
}
