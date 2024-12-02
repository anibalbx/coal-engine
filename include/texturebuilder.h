#ifndef __TEXTURE_BUILDER_H__
#define __TEXTURE_BUILDER_H__ 1

#include "types.h"

namespace coalengine
{
  class TextureBuilder
  {
  public:
    TextureBuilder();
    ~TextureBuilder();
    TextureBuilder(const TextureBuilder& o) = delete;
    TextureBuilder(TextureBuilder&& o);

    TextureBuilder& operator=(const TextureBuilder& o) = delete;
    TextureBuilder& operator=(TextureBuilder&& o);

    uint32 CreateTexture(const char* filename);

    static bool IsValidTexture(uint32 id);
  };
}

#endif

