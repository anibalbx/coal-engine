#ifndef __MATERIAL_H__
#define __MATERIAL_H__ 1

#include <glad/glad.h>
#include <string>
#include <vec4.hpp>

#include "types.h"

namespace coalengine
{
  class Texture;

  class Material
  {
  public:

    enum Type
    {
      kNone,
      kMaterial_Diffuse,
      kMaterial_PBR,
      kMaterial_Shadow,
      kMaterial_Phong,
      kMaterial_Deferred,
    };

    virtual  ~Material();

    Material();
    Material(Material& other) = delete;
    Material& operator= (Material& other) = delete;

    Material(Material&& other);
    Material& operator= (Material&& other);

    void SetColor(glm::vec4 color);

    glm::vec4 GetColor();

    void SetTexture(uint32 texture);

    Texture* GetTexture();

    virtual void SetNormalMap(uint32 texture);
    virtual Texture* GetNormalMap();

    virtual void SetMetallicMap(uint32 texture);
    virtual Texture* GetMetallicMap();

    virtual void SetRoughnessMap(uint32 texture);
    virtual Texture* GetRoughnessMap();

    virtual void SetAOMap(uint32 texture);
    virtual Texture* GetAOMap();

    virtual void SetMetallic(float metallic);
    virtual float GetMetallic();

    virtual void SetRoughness(float roughness);
    virtual float GetRoughness();

    virtual void SetAmbientOcclusion(float ao);
    virtual float GetAmbientOcclusion();

    virtual void SetIsEmissive(bool value);
    virtual bool GetIsEmissive();

    void UploadMaterial();

    Type mat_type_; /**< material type*/
    uint32 id_; /**< material id*/

    std::string vertex_source_; /**< vertex shader source */
    std::string fragment_source_; /**< fragment shader source */

    GLuint vertex_shader_id_; /**< GL vertex shader id */
    GLuint fragment_shader_id_; /**< GL fragment shader id */
    GLuint program_id_; /**< GL program id */

    Texture* texture_;

    bool uploaded_to_gpu_; /**< bool that checks if its already have been uploaded to GPU */

  protected:
    glm::vec4 color_; /**< material color data*/
  };
}

#endif
