#include "shadowmaterial.h"


coalengine::ShadowMaterial::ShadowMaterial()
{
  mat_type_ = coalengine::Material::Type::kMaterial_Shadow;
  id_ = 0;
  uploaded_to_gpu_ = false;
  fragment_shader_id_ = 0;
  vertex_shader_id_ = 0;
  program_id_ = 0;

  const char vertex_source_char[] = R"(
      #version 330 core
      layout(location = 0) in vec3 aPos;

      uniform mat4 lightSpaceMatrix;
      uniform mat4 model;

      void main()
      {
        gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
      }
     )";

  const char fragment_source_char[] = R"(
    #version 330 core
    void main()
    {
      gl_FragDepth = gl_FragCoord.z;
    }
     )";

  vertex_source_.append(vertex_source_char);
  fragment_source_.append(fragment_source_char);
}



coalengine::ShadowMaterial::ShadowMaterial(ShadowMaterial&& other)
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

coalengine::ShadowMaterial::~ShadowMaterial()
{
}

coalengine::ShadowMaterial& coalengine::ShadowMaterial::operator=(ShadowMaterial&& other)
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
