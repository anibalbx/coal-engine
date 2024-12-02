#version 330 core
     out vec4 FragColor;
     //layout (location = 1) out vec4 FragColor;

     in vec2 uv;

     uniform sampler2D u_gPosition;
     uniform sampler2D u_gNormal;
     uniform sampler2D u_gAlbedoSpec;
     uniform sampler2D u_gAmbientOcclusion;
     uniform sampler2D u_gRoughness;

     uniform vec3 u_camPos;

     uniform sampler2D u_roughnessMap;
     uniform sampler2D u_aoMap;

     uniform vec3 u_LightDirPosition[10];
     uniform vec3 u_LightDirDirection[10];
     uniform vec3 u_LightDirColor[10];
     uniform float u_LightDirIntensity[10];

     uniform vec3 u_LightPointPosition[100];
     uniform vec3 u_LightPointDirection[100];
     uniform vec3 u_LightPointColor[100];
     uniform float u_LightPointIntensity[100];

     uniform vec3 u_LightSpotPosition[10];
     uniform vec3 u_LightSpotDirection[10];
     uniform vec3 u_LightSpotColor[10];
     uniform float u_LightSpotInnerAngle[10];
     uniform float u_LightSpotOuterAngle[10];
     uniform float u_LightSpotIntensity[10];

     uniform float u_NumDirectionals;
     uniform float u_NumDirectionalsShadow;
     uniform float u_NumPoints;
     uniform float u_NumSpots;
     uniform float u_NumSpotsShadow;

     //Lights Shadows
     uniform vec3 u_LightDirPositionShadows[10];
     uniform vec3 u_LightDirDirectionShadows[10];
     uniform vec3 u_LightDirColorShadows[10];
     uniform float u_LightDirIntensityShadows[10];
     uniform mat4 u_lightSpaceMatrixDir[10];
     uniform sampler2D u_shadowMapDir[10];


     uniform vec3 u_LightPointPositionShadows[10];
     uniform vec3 u_LightPointDirectionShadows[10];
     uniform vec3 u_LightPointColorShadows[10];

     uniform vec3 u_LightSpotPositionShadows[10];
     uniform vec3 u_LightSpotDirectionShadows[10];
     uniform vec3 u_LightSpotColorShadows[10];
     uniform float u_LightSpotInnerAngleShadows[10];
     uniform float u_LightSpotOuterAngleShadows[10];
     uniform float u_LightSpotIntensityShadows[10];
     uniform mat4 u_lightSpaceMatrixSpot[10];
     uniform sampler2D u_shadowMapSpot[10];

      const float PI = 3.14159265359;

     // ----------------------------------------------------------------------------
     float DistributionGGX(vec3 N, vec3 H, float roughness)
     {
         float a = roughness*roughness;
         float a2 = a*a;
         float NdotH = max(dot(N, H), 0.0);
         float NdotH2 = NdotH*NdotH;

         float nom   = a2;
         float denom = (NdotH2 * (a2 - 1.0) + 1.0);
         denom = PI * denom * denom;

         return nom / denom;
     }
     // ----------------------------------------------------------------------------
      float GeometrySchlickGGX(float NdotV, float roughness)
     {
         float r = (roughness + 1.0);
         float k = (r*r) / 8.0;

         float nom   = NdotV;
         float denom = NdotV * (1.0 - k) + k;

         return nom / denom;
     }
     // ----------------------------------------------------------------------------
     float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
     {
       float NdotV = max(dot(N, V), 0.0);
       float NdotL = max(dot(N, L), 0.0);
       float ggx2 = GeometrySchlickGGX(NdotV, roughness);
       float ggx1 = GeometrySchlickGGX(NdotL, roughness);

       return ggx1 * ggx2;
     }

     // ----------------------------------------------------------------------------
     vec3 fresnelSchlick(float cosTheta, vec3 F0)
     {
         return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
     }
     // ----------------------------------------------------------------------------

     float DirectionalShadow(vec3 WorldPos, vec3 Normal, mat4 modelview, vec3 LightDir, sampler2D shadowMap)
     {
         vec4 FragPosLightSpace  = modelview  * vec4(WorldPos,1.0);

         vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
         projCoords = projCoords * 0.5 + 0.5;


         float currentDepth = projCoords.z;

         float closestDepth = texture(shadowMap, projCoords.xy).r;

         vec3 normal = Normal;

         vec3 lightDir = normalize(-LightDir);
         float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0005);

         float shadow = 0.0;
         vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
         for(int x = -1; x <= 1; ++x)
         {
             for(int y = -1; y <= 1; ++y)
             {
                 float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                 shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
             }
         }
         shadow /= 9.0;

         // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
         if(projCoords.z > 1.0)
             shadow = 0.0;

         return shadow;
     }

     float SpotShadow(vec3 WorldPos, vec3 Normal, mat4 modelview, vec3 LightPos, sampler2D shadowMap)
     {
         vec4 FragPosLightSpace  = modelview  * vec4(WorldPos,1.0);

         vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
         projCoords = projCoords * 0.5 + 0.5;


         float currentDepth = projCoords.z;

         float closestDepth = texture(shadowMap, projCoords.xy).r;

         vec3 normal = Normal;

         vec3 lightDir = normalize(LightPos - WorldPos);
         float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0005);

         float shadow = 0.0;
         vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
         for(int x = -1; x <= 1; ++x)
         {
             for(int y = -1; y <= 1; ++y)
             {
                 float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                 shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
             }
         }
         shadow /= 9.0;

         // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
         if(projCoords.z > 1.0)
             shadow = 0.0;

         return shadow;
     }


     vec3 DirectionalLightWithShadow(vec3 WorldPos,vec3 LightDirection, vec3 LightColor,mat4 modelview, sampler2D shadowMap,vec3 Normal, float Intensity)
     {

      vec3 albedo     = pow(texture(u_gAlbedoSpec, uv).rgb, vec3(2.2));
      float metallic  = texture(u_gAlbedoSpec, uv).a;
      float roughness = texture(u_gRoughness, uv).a;
      float ao        = texture(u_gAmbientOcclusion, uv).a;

      vec3 N = Normal;
      //vec3 N = texture(u_gNormal,uv).rgb;
      //vec3 N = normalize(normal);
      vec3 V = normalize(u_camPos - WorldPos);

      // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
      // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
      vec3 F0 = vec3(0.04);
      F0 = mix(F0, albedo, metallic);

      // reflectance equation
      vec3 Lo = vec3(0.0);

      // calculate per-light radiance
      //vec3 L = normalize(u_LightPosition - WorldPos);
      vec3 L = normalize(-LightDirection);
      vec3 H = normalize(V + L);
      vec3 radiance = LightColor * Intensity;

      // Cook-Torrance BRDF
      float NDF = DistributionGGX(N, H, roughness);
      float G   = GeometrySmith(N, V, L, roughness);
      vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

      vec3 nominator    = NDF * G * F;
      float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
      vec3 specular = nominator / denominator;

      // kS is equal to Fresnel
      vec3 kS = F;
      // for energy conservation, the diffuse and specular light can't
      // be above 1.0 (unless the surface emits light); to preserve this
      // relationship the diffuse component (kD) should equal 1.0 - kS.
      vec3 kD = vec3(1.0) - kS;
      // multiply kD by the inverse metalness such that only non-metals
      // have diffuse lighting, or a linear blend if partly metal (pure metals
      // have no diffuse light).
      kD *= 1.0 - metallic;

      // scale light by NdotL
      float NdotL = max(dot(N, L), 0.0);

      // add to outgoing radiance Lo
      Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

      // ambient lighting (note that the next IBL tutorial will replace
      // this ambient lighting with environment lighting).

      //Shadow ShadowCalculation
      float shadow;

      shadow = DirectionalShadow(WorldPos,Normal,modelview,LightDirection,shadowMap);


      Lo = mix(Lo, vec3(0.0,0.0,0.0), shadow);

      return Lo;
     }

     vec3 DirectionalLight(vec3 WorldPos,vec3 LightDirection, vec3 LightColor, vec3 Normal, float Intensity)
     {

       vec3 albedo     = pow(texture(u_gAlbedoSpec, uv).rgb, vec3(2.2));
       float metallic  = texture(u_gAlbedoSpec, uv).a;
       float roughness = texture(u_gRoughness, uv).a;
       float ao        = texture(u_gAmbientOcclusion, uv).a;

       vec3 N = Normal;
       //vec3 N = normalize(normal);
       vec3 V = normalize(u_camPos - WorldPos);

       // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
       // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
       vec3 F0 = vec3(0.04);
       F0 = mix(F0, albedo, metallic);

       // reflectance equation
       vec3 Lo = vec3(0.0);

       // calculate per-light radiance
       //vec3 L = normalize(u_LightPosition - WorldPos);
       vec3 L = normalize(-LightDirection);
       vec3 H = normalize(V + L);
       vec3 radiance = LightColor * Intensity;

       // Cook-Torrance BRDF
       float NDF = DistributionGGX(N, H, roughness);
       float G   = GeometrySmith(N, V, L, roughness);
       vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

       vec3 nominator    = NDF * G * F;
       float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
       vec3 specular = nominator / denominator;

       // kS is equal to Fresnel
       vec3 kS = F;
       // for energy conservation, the diffuse and specular light can't
       // be above 1.0 (unless the surface emits light); to preserve this
       // relationship the diffuse component (kD) should equal 1.0 - kS.
       vec3 kD = vec3(1.0) - kS;
       // multiply kD by the inverse metalness such that only non-metals
       // have diffuse lighting, or a linear blend if partly metal (pure metals
       // have no diffuse light).
       kD *= 1.0 - metallic;

       // scale light by NdotL
       float NdotL = max(dot(N, L), 0.0);

       // add to outgoing radiance Lo
       Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

       // ambient lighting (note that the next IBL tutorial will replace
       // this ambient lighting with environment lighting).

       vec3 ambient = vec3(0.03) * albedo * ao;

       vec3 color_ = ambient + Lo;

       return color_;
     }

     vec3 SpotLightWithShadow(vec3 WorldPos, vec3 LightDirection,vec3 LightPosition, vec3 LightColor, float OuterAngle, float InnerAngle,mat4 modelview,sampler2D shadowMap, vec3 Normal, float Intensity)
     {
     vec3 albedo     = pow(texture(u_gAlbedoSpec, uv).rgb, vec3(2.2));
     float metallic  = texture(u_gAlbedoSpec, uv).a;
     float roughness = texture(u_gRoughness, uv).a;
     float ao        = texture(u_gAmbientOcclusion, uv).a;

     vec3 N = Normal;
     //vec3 N = normalize(normal);
     vec3 V = normalize(u_camPos - WorldPos);

     // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
     // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
     vec3 F0 = vec3(0.04);
     F0 = mix(F0, albedo, metallic);

     // reflectance equation
     vec3 Lo = vec3(0.0);

     // calculate per-light radiance
     vec3 L = normalize(LightPosition - WorldPos);

     float theta     = dot(L, normalize(-LightDirection));
     float epsilon   = InnerAngle - OuterAngle;
     float intensity = clamp((theta - OuterAngle) / epsilon, 0.0, 1.0);

     vec3 H = normalize(V + L);
     float distance = length(LightPosition - WorldPos);
     float attenuation = 1.0 / (distance * distance);
     vec3 radiance = LightColor * attenuation * Intensity;

     // Cook-Torrance BRDF
     float NDF = DistributionGGX(N, H, roughness);
     float G   = GeometrySmith(N, V, L, roughness);
     vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

     vec3 nominator    = NDF * G * F;
     float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
     vec3 specular = nominator / denominator;

     // kS is equal to Fresnel
     vec3 kS = F;
     // for energy conservation, the diffuse and specular light can't
     // be above 1.0 (unless the surface emits light); to preserve this
     // relationship the diffuse component (kD) should equal 1.0 - kS.
     vec3 kD = vec3(1.0) - kS;
     // multiply kD by the inverse metalness such that only non-metals
     // have diffuse lighting, or a linear blend if partly metal (pure metals
     // have no diffuse light).
     kD *= 1.0 - metallic;

     // scale light by NdotL
     float NdotL = max(dot(N, L), 0.0);

     // add to outgoing radiance Lo
     Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

     // ambient lighting (note that the next IBL tutorial will replace
     // this ambient lighting with environment lighting).

     //Shadow ShadowCalculation
     float shadow;
     shadow = SpotShadow(WorldPos, Normal, modelview, LightPosition, shadowMap);

     vec3 ambient = vec3(0.03) * albedo * ao;

     vec3 color_ = ambient + Lo;

     color_ = mix(color_, vec3(0.0,0.0,0.0), shadow);
     //color_ *= shadow;
     return color_ * intensity;
     }

     vec3 SpotLight(vec3 WorldPos, vec3 LightDirection,vec3 LightPosition, vec3 LightColor, float OuterAngle, float InnerAngle, vec3 Normal, float Intensity)
     {

       vec3 albedo     = pow(texture(u_gAlbedoSpec, uv).rgb, vec3(2.2));
       float metallic  = texture(u_gAlbedoSpec, uv).a;
       float roughness = texture(u_gRoughness, uv).a;
       float ao        = texture(u_gAmbientOcclusion, uv).a;

       vec3 N = Normal;
       //vec3 N = normalize(normal);
       vec3 V = normalize(u_camPos - WorldPos);

       // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
       // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
       vec3 F0 = vec3(0.04);
       F0 = mix(F0, albedo, metallic);

       // reflectance equation
       vec3 Lo = vec3(0.0);

       // calculate per-light radiance
       vec3 L = normalize(LightPosition - WorldPos);

       float theta     = dot(L, normalize(-LightDirection));
       float epsilon   = InnerAngle - OuterAngle;
       float intensity = clamp((theta - OuterAngle) / epsilon, 0.0, 1.0);

       vec3 H = normalize(V + L);
       float distance = length(LightPosition - WorldPos);
       float attenuation = 1.0 / (distance * distance);
       vec3 radiance = LightColor * attenuation * Intensity;

       // Cook-Torrance BRDF
       float NDF = DistributionGGX(N, H, roughness);
       float G   = GeometrySmith(N, V, L, roughness);
       vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

       vec3 nominator    = NDF * G * F;
       float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
       vec3 specular = nominator / denominator;

       // kS is equal to Fresnel
       vec3 kS = F;
       // for energy conservation, the diffuse and specular light can't
       // be above 1.0 (unless the surface emits light); to preserve this
       // relationship the diffuse component (kD) should equal 1.0 - kS.
       vec3 kD = vec3(1.0) - kS;
       // multiply kD by the inverse metalness such that only non-metals
       // have diffuse lighting, or a linear blend if partly metal (pure metals
       // have no diffuse light).
       kD *= 1.0 - metallic;

       // scale light by NdotL
       float NdotL = max(dot(N, L), 0.0);

       // add to outgoing radiance Lo
       Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

       // ambient lighting (note that the next IBL tutorial will replace
       // this ambient lighting with environment lighting).

       vec3 ambient = vec3(0.03) * albedo * ao;

       vec3 color_ = ambient + Lo;


       return color_ * intensity;
     }

     vec3 PointLight(vec3 WorldPos, vec3 LightPosition, vec3 LightColor,vec3 Normal, float Intensity)
     {

     vec3 albedo     = pow(texture(u_gAlbedoSpec, uv).rgb, vec3(2.2));
     float metallic  = texture(u_gAlbedoSpec, uv).a;
     float roughness = texture(u_gRoughness, uv).a;
     float ao        = texture(u_gAmbientOcclusion, uv).a;

     vec3 N = Normal;
     //vec3 N = normalize(normal);
     vec3 V = normalize(u_camPos - WorldPos);

     // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
     // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
     vec3 F0 = vec3(0.04);
     F0 = mix(F0, albedo, metallic);

     // reflectance equation
     vec3 Lo = vec3(0.0);

     // calculate per-light radiance
     vec3 L = normalize(LightPosition - WorldPos);
     vec3 H = normalize(V + L);
     float distance = length(LightPosition - WorldPos);
     float attenuation = 1.0 / (distance * distance);
     vec3 radiance = LightColor * attenuation * Intensity;

     // Cook-Torrance BRDF
     float NDF = DistributionGGX(N, H, roughness);
     float G   = GeometrySmith(N, V, L, roughness);
     vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

     vec3 nominator    = NDF * G * F;
     float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
     vec3 specular = nominator / denominator;

     // kS is equal to Fresnel
     vec3 kS = F;
     // for energy conservation, the diffuse and specular light can't
     // be above 1.0 (unless the surface emits light); to preserve this
     // relationship the diffuse component (kD) should equal 1.0 - kS.
     vec3 kD = vec3(1.0) - kS;
     // multiply kD by the inverse metalness such that only non-metals
     // have diffuse lighting, or a linear blend if partly metal (pure metals
     // have no diffuse light).
     kD *= 1.0 - metallic;

     // scale light by NdotL
     float NdotL = max(dot(N, L), 0.0);

     // add to outgoing radiance Lo
     return Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

     // ambient lighting (note that the next IBL tutorial will replace
     // this ambient lighting with environment lighting).
     }

     void main()
     {
         // retrieve data from gbuffer
         vec3 albedo   = pow(texture(u_gAlbedoSpec, uv).rgb, vec3(2.2));
         vec3 FragPos = texture(u_gPosition, uv).rgb;
         vec3 Normal = texture(u_gNormal, uv).rgb;
         float Metallic = texture(u_gAlbedoSpec, uv).a;
         float AO = texture(u_gAmbientOcclusion, uv).a;
         float Roughness = texture(u_gRoughness, uv).a;

        vec3 lighting = vec3(0.0f,0.0f,0.0f);

         for(int i = 0; i < u_NumDirectionals; i++)
         {
           lighting += DirectionalLight(FragPos,u_LightDirDirection[i],u_LightDirColor[i],Normal, u_LightDirIntensity[i]);
         }

         for(int i = 0; i < u_NumDirectionalsShadow; i++)
         {
           lighting += DirectionalLightWithShadow(FragPos,u_LightDirDirectionShadows[i],u_LightDirColorShadows[i],u_lightSpaceMatrixDir[i],u_shadowMapDir[i],Normal,u_LightDirIntensityShadows[i]);
         }

         for(int i = 0; i < u_NumSpots; i++)
         {
            lighting += SpotLight(FragPos,u_LightSpotDirection[i],u_LightSpotPosition[i],u_LightSpotColor[i],u_LightSpotOuterAngle[i],u_LightSpotInnerAngle[i],Normal,u_LightSpotIntensity[i]);
         }

         for(int i = 0; i < u_NumSpotsShadow; i++)
         {
             lighting += SpotLightWithShadow(FragPos,u_LightSpotDirectionShadows[i],u_LightSpotPositionShadows[i],u_LightSpotColorShadows[i],u_LightSpotOuterAngleShadows[i],u_LightSpotInnerAngleShadows[i],u_lightSpaceMatrixSpot[i],u_shadowMapSpot[i],Normal,u_LightSpotIntensityShadows[i]);
         }

         for(int i = 0; i < u_NumPoints; i++)
         {
             lighting += PointLight(FragPos,u_LightPointPosition[i],u_LightPointColor[i],Normal,u_LightPointIntensity[i]);
         }

        vec3 ambient = vec3(0.0) * albedo * AO;

        vec3 color = ambient + lighting;
        // HDR tonemapping
        color = color / (color + vec3(1.0));

        vec3 mapped = vec3(1.0) - exp(-color * 2.0);
        // gamma correct
        color = pow(mapped, vec3(1.0/2.2));

        FragColor = vec4(color, 1.0);
        //FragColor = vec4(vec3(Roughness,Roughness,Roughness), 1.0);

    }
