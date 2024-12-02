#include "cubemap.h"

coalengine::CubeMap::CubeMap()
{
  vertex_shader_id_ = 0;
  fragment_shader_id_ = 0;
  program_id_ = 0;

  texture_id_ = 0;
  id_ = 0;
  vao_ = 0;
  triangle_vertices_ID = 0;
  triangle_indices_ID = 0;

  uploaded_to_gpu_ = false;
}


coalengine::CubeMap::CubeMap(CubeMap&& o)
{
  vertex_shader_id_ = o.vertex_shader_id_;
  fragment_shader_id_ = o.fragment_shader_id_;
  program_id_ = o.program_id_;

  texture_id_ = o.texture_id_;
  id_ = o.id_;
  vao_ = o.vao_;
  triangle_vertices_ID = o.triangle_vertices_ID;
  triangle_indices_ID = o.triangle_indices_ID;

  uploaded_to_gpu_ = o.uploaded_to_gpu_;
}

coalengine::CubeMap::~CubeMap()
{

}

coalengine::CubeMap& coalengine::CubeMap::operator=(CubeMap&& o)
{
  vertex_shader_id_ = o.vertex_shader_id_;
  fragment_shader_id_ = o.fragment_shader_id_;
  program_id_ = o.program_id_;

  texture_id_ = o.texture_id_;
  id_ = o.id_;
  vao_ = o.vao_;
  triangle_vertices_ID = o.triangle_vertices_ID;
  triangle_indices_ID = o.triangle_indices_ID;

  uploaded_to_gpu_ = o.uploaded_to_gpu_;

  return  *this;
}
