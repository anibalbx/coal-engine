#include "deferredmaterial.h"

coalengine::DeferredMaterial::DeferredMaterial()
{
  mat_type_ = kMaterial_Deferred;
  id_ = 0;
  uploaded_to_gpu_ = false;
  fragment_shader_id_ = 0;
  vertex_shader_id_ = 0;
  program_id_ = 0;

  const char vertex_source_char[] = R"(
       #version 330 core
       layout (location = 0) in vec3 a_position;
       layout (location = 1) in vec3 a_normal;
       layout (location = 2) in vec2 a_uv;
       
       out vec3 WorldPos;
       out vec2 uv;
       out vec3 normal;
       
       uniform mat4 u_model;
       uniform mat4 u_mvp;
       uniform mat4 u_view;
       
       void main()
       {
           vec4 Pos = u_model * vec4(a_position, 1.0);
           WorldPos = Pos.xyz;
           uv = a_uv;
           normal = mat3(u_model) * a_normal;
           gl_Position = u_mvp * vec4(a_position,1.0);
       }
     )";

  const char fragment_source_char[] = R"(
    #version 330 core
    layout (location = 0) out vec3 gPosition;
    layout (location = 1) out vec3 gNormal;
    layout (location = 2) out vec4 gAlbedoSpec;
    layout (location = 3) out vec4 gAmbientOcclusion;
    layout (location = 4) out vec4 gRoughness;
    
    in vec2 uv;
    in vec3 WorldPos;
    in vec3 normal;
    
    uniform sampler2D texture_diffuse1;
    uniform sampler2D texture_specular1;
    uniform sampler2D texture_ao;
    uniform sampler2D u_normalMap;
    uniform sampler2D texture_roughness;

    uniform vec3 u_color;

	uniform bool u_HasNormalMap;

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

    void main()
    {             
        gPosition = WorldPos;

		    if(u_HasNormalMap)
		    {
		    	gNormal = getNormalFromMap();
		    }

		    if(!u_HasNormalMap)
		    {
		    	gNormal = normal;
		    }

        gAlbedoSpec.rgb = texture(texture_diffuse1, uv).rgb + u_color;

        gAlbedoSpec.a = texture(texture_specular1, uv).r;

        gAmbientOcclusion.a = texture(texture_ao, uv).r;

        gRoughness.a = texture(texture_roughness, uv).r;
    }  
   )";

  vertex_source_.append(vertex_source_char);
  fragment_source_.append(fragment_source_char);
}

coalengine::DeferredMaterial::DeferredMaterial(DeferredMaterial&& other)
{
  mat_type_ = other.mat_type_;
  id_ = other.id_;
  uploaded_to_gpu_ = other.uploaded_to_gpu_;
  fragment_shader_id_ = other.fragment_shader_id_;
  vertex_shader_id_ = other.fragment_shader_id_;
  program_id_ = other.program_id_;
  vertex_source_ = other.vertex_source_;
  fragment_source_ = other.fragment_source_;
}

coalengine::DeferredMaterial::~DeferredMaterial()
{
}

coalengine::DeferredMaterial& coalengine::DeferredMaterial::operator=(DeferredMaterial&& other)
{
  mat_type_ = other.mat_type_;
  id_ = other.id_;
  uploaded_to_gpu_ = other.uploaded_to_gpu_;
  fragment_shader_id_ = other.fragment_shader_id_;
  vertex_shader_id_ = other.fragment_shader_id_;
  program_id_ = other.program_id_;
  vertex_source_ = other.vertex_source_;
  fragment_source_ = other.fragment_source_;
  return  *this;
}
