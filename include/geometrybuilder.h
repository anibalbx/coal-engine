#ifndef __GEOMETRY_BUILDER_H__
#define __GEOMETRY_BUILDER_H__ 1

#include <vec3.hpp>
#include <vector>
#include "types.h"
#include <string>

#include "geometry.h"
#include "material.h"
#include "glm/glm.hpp"

namespace coalengine
{
  class GeometryBuilder
  {
  public:
    GeometryBuilder();
    GeometryBuilder(const GeometryBuilder& other) = delete;
    GeometryBuilder(GeometryBuilder&& other);
    ~GeometryBuilder();

    GeometryBuilder& operator=(const GeometryBuilder& other) = delete;
    GeometryBuilder& operator=(GeometryBuilder&& other);

    // Add the vertices to the current geometry is building.
    void AddVertex(const float* vertex, unsigned int size);

    // Add the index to the current geometry is building.
    void AddIndex(const unsigned int* index, unsigned int size);

    // Add the normals to the current geometry is building.
    void AddNormals(const float* normals, unsigned int size);

    // Add the uvs to the current geometry is building.
    void AddUV(const float* uv, unsigned int size);


    void SetMaterial(uint32 mat);

    void SetMaterial(uint32 geo, uint32 mat);

    uint32 GetMaterial(uint32 id);

    glm::mat4 GetTransform(uint32 id);

    uint32 LoadObj(std::string inputfile);

    uint32 Finish();

    Material* mat_;

    void SetShape(Shape shape);

    std::vector<float> vertices_;
    std::vector<float> normals_;
    std::vector<float> uv_;
    std::vector<unsigned int> index_;

    Shape shape_;

  private:

    void ResetBuilder();
  };
}

#endif

