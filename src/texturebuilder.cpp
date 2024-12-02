#include  "texturebuilder.h"
#include <px_mem.h>
#include "RenderList.h"
#include "texture.h"

coalengine::TextureBuilder::TextureBuilder()
{
}

coalengine::TextureBuilder::~TextureBuilder()
{
}

coalengine::TextureBuilder::TextureBuilder(TextureBuilder&& o)
{
}

coalengine::TextureBuilder& coalengine::TextureBuilder::operator=(TextureBuilder&& o)
{
  return *this;
}

uint32 coalengine::TextureBuilder::CreateTexture(const char* filename)
{
  px::Mem<Texture> texture;
  texture.alloc();

  texture->LoadTextureFileName(filename);

  uint32 id = RenderList::Instance().num_textures_;
  RenderList::Instance().num_textures_++;

  id = id | 0x20000000;
  texture->id_ = id;
  RenderList::Instance().SubmitTexture(std::move(texture));
  return id;
}

bool coalengine::TextureBuilder::IsValidTexture(uint32 id)
{
  if (RenderList::Instance().IsTexture(id))
  {
    std::map<uint32, px::Mem<Texture>>::iterator it =
      RenderList::Instance().texture_map_.find(id);

    if (it != RenderList::Instance().texture_map_.end())
    {
      return true;
    }
  }
  return false;
}
