#define STB_IMAGE_IMPLEMENTATION 1
#include "texture.h"
#include "stb_image.h"

coalengine::Texture::Texture()
{
  texture_id_ = 0;
  uploaded_to_gpu_ = false;
  loaded_ = false;
  id_ = 0;
  texture_data_ = nullptr;
  width_ = 0;
  height_ = 0;
  nr_channels_ = 0;
}

coalengine::Texture::Texture(Texture&& other)
{
  texture_id_ = other.texture_id_;
}

coalengine::Texture::~Texture()
{
  glDeleteTextures(1, &texture_id_);
}

coalengine::Texture& coalengine::Texture::operator=(Texture&& other)
{
  texture_id_ = other.texture_id_;
  return  *this;
}

void coalengine::Texture::LoadTextureFileName(const char* filename)
{
  filename_ = filename;

}

bool coalengine::Texture::LoadTexture()
{
  if (filename_.c_str() != nullptr && !loaded_) 
  {
    texture_data_ = stbi_load(filename_.c_str(), &width_, &height_, &nr_channels_, STBI_rgb);

    if (!texture_data_)
    {
      printf("fail stbi_load\n");
      return false;
    }
    loaded_ = true;
    return true;
  }

  printf("texture path error\n");
  return false;
}

void coalengine::Texture::UploadTexture()
{
  if(!uploaded_to_gpu_ && texture_data_ != nullptr)
  {


    glGenTextures(1, &texture_id_);
    glActiveTexture(GL_TEXTURE0 + texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
      width_, height_, 0, GL_RGB,
      GL_UNSIGNED_BYTE, texture_data_);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(texture_data_);

    uploaded_to_gpu_ = true;
  }
}


