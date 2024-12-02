#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__ 1

#include <glad/glad.h>
#include <vector>
#include "material.h"

namespace coalengine
{

  class CubeMap
  {
  public:
 
    CubeMap();

    CubeMap(const CubeMap& other) = delete;

    CubeMap(CubeMap&& other);

    ~CubeMap();
 
    CubeMap& operator=(const CubeMap& other) = delete;

    CubeMap& operator=(CubeMap&& other);

   
    std::vector<float> vertices_; /**< Vector that stores all the vertex*/
    std::vector<float> normals_; /**< Vector that stores all the vertex*/
    std::vector<float> uv_; /**< Vector that stores all the vertex*/
    std::vector<unsigned int> index_; /**< Vector that stores all the index*/


    std::vector<std::string> textures;

    std::string vertex_source_; /**< vertex shader source */
    std::string fragment_source_; /**< fragment shader source */
    GLuint vertex_shader_id_;
    GLuint fragment_shader_id_;
    GLuint program_id_;

    GLuint texture_id_;
    uint32 id_; /**< geometry id*/
    GLuint vao_; /**< GL vertex array object id */
    GLuint triangle_vertices_ID; /**< GL vertex buffer id */
    GLuint triangle_indices_ID; /**< GL index buffer id */
   
    bool uploaded_to_gpu_; /**< bool that checks if its already have been uploaded to GPU */
  };
}

#endif

