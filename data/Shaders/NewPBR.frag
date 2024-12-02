#version 330 core
layout (location = 0)out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
in vec3 normal;
in vec4 color;
in vec2 uv;
in vec3 WorldPos;

//Camera
uniform vec3 u_camPos;

// Material parameters
vec3 albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ambient_occlusion;

// Textures
uniform sampler2D my_texture;
uniform sampler2D u_normalMap;
uniform sampler2D u_metallicMap;
uniform sampler2D u_roughnessMap;
uniform sampler2D u_aoMap;
uniform sampler2D u_shadowMap[10];

uniform float u_NumDirectionals;
uniform float u_NumDirectionalsShadow;
uniform float u_NumPoints;
uniform float u_NumSpots;
uniform float u_NumSpotsShadow;

//Lights No Shadows
uniform vec3 u_LightDirPosition[10];
uniform vec3 u_LightDirDirection[10];
uniform vec3 u_LightDirColor[10];
uniform float u_LightDirIntensity[10];

uniform vec3 u_LightPointPosition[10];
uniform vec3 u_LightPointDirection[10];
uniform vec3 u_LightPointColor[10];
uniform float u_LightPointIntensity[10];

uniform vec3 u_LightSpotPosition[10];
uniform vec3 u_LightSpotDirection[10];
uniform vec3 u_LightSpotColor[10];
uniform float u_LightSpotInnerAngle[10];
uniform float u_LightSpotOuterAngle[10];
uniform float u_LightSpotIntensity[10];

//Lights Shadows
uniform vec3 u_LightDirPositionShadows[10];
uniform vec3 u_LightDirDirectionShadows[10];
uniform vec3 u_LightDirColorShadows[10];
uniform float u_LightDirIntensityShadows[10];
in vec4 FragLightPosDir[10];
uniform sampler2D u_shadowMapDir[10];
in float num_shadows_dir;

uniform vec3 u_LightPointPositionShadows[10];
uniform vec3 u_LightPointDirectionShadows[10];
uniform vec3 u_LightPointColorShadows[10];
in vec4 FragLightPosPoint[10];

uniform vec3 u_LightSpotPositionShadows[10];
uniform vec3 u_LightSpotDirectionShadows[10];
uniform vec3 u_LightSpotColorShadows[10];
uniform float u_LightSpotInnerAngleShadows[10];
uniform float u_LightSpotOuterAngleShadows[10];
uniform float u_LightSpotIntensityShadows[10];
in vec4 FragLightPosSpot[10];
uniform sampler2D u_shadowMapSpot[10];
in float num_shadows_spot;

//posprocess
uniform bool u_activebloom;


const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
 vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_normalMap, uv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


float ShadowDirectionalCalculation(vec4 fragPosLightSpace, vec3 LightDir, sampler2D shadowMap)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(-LightDir);
    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0005);

    // PCF
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

 float ShadowSpotCalculation(vec4 fragPosLightSpace, vec3 LightPos, sampler2D shadowMap)
{
   /// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    //float closestDepth = textureProj(shadowMap, fragPosLightSpace);
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(LightPos - WorldPos);
    float bias = max(0.00005 * (1.0 - dot(normal, lightDir)), 0.00005);

    // check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
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


 vec3 CalculatePoint(vec3 LightPosition, vec3 LightColor, float Intensity)
{

  vec3 albedo     = pow(texture(my_texture, uv).rgb, vec3(2.2));
  float metallic  = texture(u_metallicMap, uv).r;
  float roughness = texture(u_roughnessMap, uv).r;
  float ao        = texture(u_aoMap, uv).r;

  vec3 N = getNormalFromMap();
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
  Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

  // ambient lighting (note that the next IBL tutorial will replace
  // this ambient lighting with environment lighting).


  vec3 ambient = vec3(0.03) * albedo * ao;

  vec3 color_ = ambient + Lo;

  return color_;
}


void main()
{

    //WorldPos = texture(u_gPosition,uv).rgb;

    vec3 color = vec3(0.0,0.0,0.0);

    for(int i = 0; i < u_NumPoints; i++)
    {
        color += CalculatePoint(u_LightPointPosition[i],u_LightPointColor[i],u_LightPointIntensity[i]);
    }

    if(!u_activebloom){
      // HDR tonemapping
       color = color / (color + vec3(1.0));

      vec3 mapped = vec3(1.0) - exp(-color * 2.0);
      // gamma correct
      color = pow(mapped, vec3(1.0/2.2));
    }

    FragColor = vec4(color, 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
       BrightColor = vec4(FragColor.rgb, 1.0);
     else
       BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}
