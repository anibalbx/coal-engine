#include <cstdio>
#include <material.h>

#include "renderlist.h"
#include "texturebuilder.h"
#include "texture.h"

coalengine::Material::~Material()
{
  glDeleteShader(fragment_shader_id_);
  glDeleteShader(vertex_shader_id_);
  glDeleteProgram(program_id_);
}

coalengine::Material::Material()
{
  mat_type_ = kNone;
  id_ = 0;
  vertex_source_ = "";
  fragment_source_ = "";
  vertex_shader_id_ = 0;
  fragment_shader_id_ = 0;
  program_id_ = 0;
  texture_ = nullptr;
  uploaded_to_gpu_ = false;
  color_.x = 0.0f;
  color_.y = 0.0f;
  color_.z = 0.0f;
  color_.w = 0.0f;
}

coalengine::Material::Material(Material&& other)
{
  mat_type_ = other.mat_type_;
  id_ = other.id_;
  vertex_source_ = other.vertex_source_;
  fragment_source_ = other.fragment_source_;
  vertex_shader_id_ = other.vertex_shader_id_;
  fragment_shader_id_ = other.fragment_shader_id_;
  program_id_ = other.program_id_;
  texture_ = other.texture_;
  uploaded_to_gpu_ = other.uploaded_to_gpu_;
  color_.x = other.color_.x;
  color_.y = other.color_.y;
  color_.z = other.color_.z;
  color_.w = other.color_.w;
}

coalengine::Material& coalengine::Material::operator=(Material&& other)
{
  mat_type_ = other.mat_type_;
  id_ = other.id_;
  vertex_source_ = other.vertex_source_;
  fragment_source_ = other.fragment_source_;
  vertex_shader_id_ = other.vertex_shader_id_;
  fragment_shader_id_ = other.fragment_shader_id_;
  program_id_ = other.program_id_;
  texture_ = other.texture_;
  uploaded_to_gpu_ = other.uploaded_to_gpu_;
  color_.x = other.color_.x;
  color_.y = other.color_.y;
  color_.z = other.color_.z;
  color_.w = other.color_.w;
  return *this;

}

void coalengine::Material::SetColor(glm::vec4 color)
{
  color_.x = color.x;
  color_.y = color.y;
  color_.z = color.z;
  color_.w = color.w;
}

glm::vec4 coalengine::Material::GetColor()
{
  return color_;
}

void coalengine::Material::SetTexture(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    texture_ = RenderList::Instance().texture_map_[texture].get();
  }
}

coalengine::Texture* coalengine::Material::GetTexture()
{
  return texture_;
}

void coalengine::Material::SetNormalMap(uint32 texture)
{
}

coalengine::Texture* coalengine::Material::GetNormalMap()
{
  return nullptr;
}

void coalengine::Material::SetMetallicMap(uint32 texture)
{
}

coalengine::Texture* coalengine::Material::GetMetallicMap()
{
  return nullptr;
}

void coalengine::Material::SetRoughnessMap(uint32 texture)
{
}

coalengine::Texture* coalengine::Material::GetRoughnessMap()
{
  return nullptr;
}

void coalengine::Material::SetAOMap(uint32 texture)
{

}

coalengine::Texture* coalengine::Material::GetAOMap()
{
  return nullptr;
}

void coalengine::Material::SetMetallic(float metallic)
{

}

float coalengine::Material::GetMetallic()
{
  return 0.0f;
}

void coalengine::Material::SetRoughness(float roughness)
{
}

float coalengine::Material::GetRoughness()
{
  return 0.0f;
}

void coalengine::Material::SetAmbientOcclusion(float ao)
{
}

float coalengine::Material::GetAmbientOcclusion()
{
  return 0.0f;
}

void coalengine::Material::SetIsEmissive(bool value)
{
}

bool coalengine::Material::GetIsEmissive()
{
  return true;
}

void coalengine::Material::UploadMaterial()
{
  GLenum error = glGetError();

  if (error != GL_NO_ERROR)
  {
    printf("OPENGL error");
  }

  if (!uploaded_to_gpu_)
  {
    // Vertex Shader
    vertex_shader_id_ = glCreateShader(GL_VERTEX_SHADER);

    int length = vertex_source_.size();
    const char* c_str = vertex_source_.c_str();
    glShaderSource(vertex_shader_id_, 1, &c_str, &length);
    glCompileShader(vertex_shader_id_);

    // Fragment Shader
    fragment_shader_id_ = glCreateShader(GL_FRAGMENT_SHADER);

    length = fragment_source_.size();
    c_str = fragment_source_.c_str();
    glShaderSource(fragment_shader_id_, 1, &c_str, &length);
    glCompileShader(fragment_shader_id_);

    //program
    program_id_ = glCreateProgram();
    glAttachShader(program_id_, vertex_shader_id_);
    glAttachShader(program_id_, fragment_shader_id_);
    glLinkProgram(program_id_);
    glUseProgram(program_id_);

    // Compile and link check
    int success;

    glGetShaderiv(vertex_shader_id_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      printf("compile vertex error\n");
    }

    glGetShaderiv(fragment_shader_id_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      printf("compile fragment error\n");
    }

    glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
    if (!success)
    {

      printf("Program link Error\n");
    }

    uploaded_to_gpu_ = true;
  }

  if (error != GL_NO_ERROR)
  {
    printf("OPENGL error");
  }
}
