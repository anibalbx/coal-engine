#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__ 1

#include <glad/glad.h>
#include <vec3.hpp>
#include <vector>
#include "material.h"
#include "glm/glm.hpp"

namespace coalengine
{
  enum class Shape
  {
    kCube,
    kSphere,
    kPlane,
    kOther,
  };

  class Geometry
  {
  public:


    Geometry();

    Geometry(const Geometry& other) = delete;

    Geometry(Geometry&& other);

    ~Geometry();

    Geometry& operator=(const Geometry& other) = delete;

    Geometry& operator=(Geometry&& other);


    void RenderGeometry();

    std::vector<float> vertices_; /**< Vector that stores all the vertex*/
    std::vector<float> normals_; /**< Vector that stores all the vertex*/
    std::vector<float> uv_; /**< Vector that stores all the vertex*/
    std::vector<unsigned int> index_; /**< Vector that stores all the index*/


    uint32 id_; /**< geometry id*/
    GLuint vao_; /**< GL vertex array object id */
    GLuint triangle_vertices_ID; /**< GL vertex buffer id */
    GLuint triangle_indices_ID; /**< GL index buffer id */
    glm::mat4 transform_;
    Material* mat_; /**< Pointer to the geometries material */

    bool uploaded_to_gpu_; /**< bool that checks if its already have been uploaded to GPU */

    Shape shape_;
  };
}

#endif
