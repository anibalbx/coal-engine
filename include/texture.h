#ifndef __TEXTURE_H__
#define __TEXTURE_H__ 1

#include <glad/glad.h>
#include "types.h"
#include <string>

namespace coalengine
{

  class Texture
  {
  public:

    Texture();

    Texture(const Texture& other) = delete;

    Texture(Texture&& other);

    ~Texture();

    Texture& operator= (const Texture& other) = delete;

    Texture& operator= (Texture&& other);

    // Load texture from file
    void LoadTextureFileName(const char* filename);

    bool LoadTexture();

    // Load texture to buffer.
    void UploadTexture();

    bool loaded_;
    bool uploaded_to_gpu_; /**< bool that checks if its already have been uploaded to GPU */
    unsigned char* texture_data_; /**< texture data */
    GLuint texture_id_; /**< GL texture id */
    int width_; /**< texture width */
    int height_; /**< texture height */
    int nr_channels_; /**< texture channels */
    uint32 id_; /**< texture id */
    std::string filename_;
  };
}

#endif

