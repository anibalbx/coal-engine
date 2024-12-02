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
      const vec3 Fdielectric = vec3(0.04);

      // GGX/Towbridge-Reitz normal distribution function.
      // Uses Disney's reparametrization of alpha = roughness^2.
      float ndfGGX(float cosLh, float roughness)
      {
      	float alpha   = roughness * roughness;
      	float alphaSq = alpha * alpha;

      	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
      	return alphaSq / (PI * denom * denom);
      }

      // Single term for separable Schlick-GGX below.
      float gaSchlickG1(float cosTheta, float k)
      {
      	return cosTheta / (cosTheta * (1.0 - k) + k);
      }

      // Schlick-GGX approximation of geometric attenuation function using Smith's method.
      float gaSchlickGGX(float cosLi, float cosLo, float roughness)
      {
      	float r = roughness + 1.0;
      	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
      	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
      }

      // Shlick's approximation of the Fresnel factor.
      vec3 fresnelSchlick(vec3 F0, float cosTheta)
      {
      	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
      }


     vec3 DirectionalLight(vec3 WorldPos,vec3 LightDirection, vec3 LightColor, vec3 Normal, float Intensity)
     {

       vec3 albedo     = pow(texture(u_gAlbedoSpec, uv).rgb, vec3(2.2));
       float metallic  = texture(u_gAlbedoSpec, uv).a;
       float roughness = texture(u_gRoughness, uv).a;
       float ao        = texture(u_gAmbientOcclusion, uv).a;

       // Outgoing light direction (vector from world-space fragment position to the "eye").
	      vec3 Lo = normalize(u_camPos - WorldPos);

	      // Get current fragment's normal and transform to world space.
	      vec3 N = normalize(Normal);

	      // Angle between surface normal and outgoing light direction.
	      float cosLo = max(0.0, dot(N, Lo));

	      // Specular reflection vector.
	      vec3 Lr = 2.0 * cosLo * N - Lo;

	      // Fresnel reflectance at normal incidence (for metals use albedo color).
	      vec3 F0 = mix(Fdielectric, albedo, metallic);

	      //Direct lighting calculation for analytical lights.
	      vec3 directLighting = vec3(0.0);

	      vec3 Li = -LightDirection;
	      vec3 Lradiance = LightColor;

	      // Half-vector between Li and Lo.
	      vec3 Lh = normalize(Li + Lo);

	      // Calculate angles between surface normal and various light vectors.
	      float cosLi = max(0.0, dot(N, Li));
	      float cosLh = max(0.0, dot(N, Lh));

	      // Calculate Fresnel term for direct lighting.
	      vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
	      // Calculate normal distribution for specular BRDF.
	      float D = ndfGGX(cosLh, roughness);
	      // Calculate geometric attenuation for specular BRDF.
	      float G = gaSchlickGGX(cosLi, cosLo, roughness);

	      // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
	      // Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
	      // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
	      vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

	      // Lambert diffuse BRDF.
	      // We don't scale by 1/PI for lighting & material units to be more convenient.
	      // See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
	      vec3 diffuseBRDF = kd * albedo;

	      // Cook-Torrance specular microfacet BRDF.
	      vec3 specularBRDF = (F * D * G) / max(0.00001, 4.0 * cosLi * cosLo);

	      // Total contribution for this light.
	      return directLighting = (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
        //return vec3(0);
     }

     vec3 PointLight(vec3 WorldPos,vec3 LightPosition, vec3 LightColor, vec3 Normal, float Intensity)
     {

       vec3 albedo     = pow(texture(u_gAlbedoSpec, uv).rgb, vec3(2.2));
       float metallic  = texture(u_gAlbedoSpec, uv).a;
       float roughness = texture(u_gRoughness, uv).a;
       float ao        = texture(u_gAmbientOcclusion, uv).a;

       // Outgoing light direction (vector from world-space fragment position to the "eye").
	      vec3 Lo = normalize(u_camPos - WorldPos);

	      // Get current fragment's normal and transform to world space.
	      vec3 N = normalize(Normal);

	      // Angle between surface normal and outgoing light direction.
	      float cosLo = max(0.0, dot(N, Lo));

	      // Specular reflection vector.
	      vec3 Lr = 2.0 * cosLo * N - Lo;

	      // Fresnel reflectance at normal incidence (for metals use albedo color).
	      vec3 F0 = mix(Fdielectric, albedo, metallic);

	      //Direct lighting calculation for analytical lights.
	      vec3 directLighting = vec3(0.0);

	      vec3 Li = vec3 (LightPosition - WorldPos);
	      vec3 Lradiance = LightColor;

	      // Half-vector between Li and Lo.
	      vec3 Lh = normalize(Li + Lo);

	      // Calculate angles between surface normal and various light vectors.
	      float cosLi = max(0.0, dot(N, Li));
	      float cosLh = max(0.0, dot(N, Lh));

	      // Calculate Fresnel term for direct lighting.
	      vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
	      // Calculate normal distribution for specular BRDF.
	      float D = ndfGGX(cosLh, roughness);
	      // Calculate geometric attenuation for specular BRDF.
	      float G = gaSchlickGGX(cosLi, cosLo, roughness);

	      // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
	      // Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
	      // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
	      vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

	      // Lambert diffuse BRDF.
	      // We don't scale by 1/PI for lighting & material units to be more convenient.
	      // See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
	      vec3 diffuseBRDF = kd * albedo;

	      // Cook-Torrance specular microfacet BRDF.
	      vec3 specularBRDF = (F * D * G) / max(0.00001, 4.0 * cosLi * cosLo);

	      // Total contribution for this light.
	      return directLighting = (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
        //return vec3(0);
     }

     void main()
     {
         // retrieve data from gbuffer
         vec3 FragPos = texture(u_gPosition, uv).rgb;
         vec3 Normal = texture(u_gNormal, uv).rgb;
         float Metallic = texture(u_gAlbedoSpec, uv).a;
         float AO = texture(u_gAmbientOcclusion, uv).a;
         float Roughness = texture(u_gRoughness, uv).a;

         vec3 lighting = vec3(0.0f,0.0f,0.0f);

         for(int i = 0; i < u_NumDirectionalsShadow; i++)
         {
           lighting += DirectionalLight(FragPos,u_LightDirDirectionShadows[i],u_LightDirColorShadows[i],Normal, u_LightDirIntensityShadows[i]);
         }

         for(int i = 0; i < u_NumPoints; i++)
         {
           lighting += PointLight(FragPos,u_LightPointPosition[i],u_LightPointColor[i],Normal, u_LightDirIntensityShadows[i]);
         }

        // HDR tonemapping
        lighting = lighting / (lighting + vec3(1.0));

        vec3 mapped = vec3(1.0) - exp(-lighting * 2.0);
        // gamma correct
        lighting = pow(mapped, vec3(1.0/2.2));

        FragColor = vec4(lighting, 1.0);
        //FragColor = vec4(1.0,0.0,0.0, 1.0);
        //ragColor = vec4(vec3(Roughness,Roughness,Roughness), 1.0);

    }
