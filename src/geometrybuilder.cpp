#define TINYOBJLOADER_IMPLEMENTATION 1

#include "geometrybuilder.h"
#include <px_mem.h>
#include "geometry.h"
#include "materialbuilder.h"
#include "RenderList.h"
#include "tiny_obj_loader.h"
#include <stdio.h>


coalengine::GeometryBuilder::GeometryBuilder()
{
  mat_ = nullptr;
}

coalengine::GeometryBuilder::GeometryBuilder(GeometryBuilder&& other)
{
  vertices_ = std::move(other.vertices_);
  normals_ = std::move(other.normals_);
  uv_ = std::move(other.uv_);
  index_ = std::move(other.index_);
}

coalengine::GeometryBuilder::~GeometryBuilder()
{
}

coalengine::GeometryBuilder& coalengine::GeometryBuilder::operator=(GeometryBuilder&& other)
{
  vertices_.clear();
  normals_.clear();
  uv_.clear();
  index_.clear();
  vertices_ = std::move(other.vertices_);
  normals_ = std::move(other.normals_);
  uv_ = std::move(other.uv_);
  index_ = std::move(other.index_);
  return  *this;
}

void coalengine::GeometryBuilder::AddVertex(const float* vertex, unsigned int size)
{
  for (unsigned int i = 0; i < size; i++)
  {
    vertices_.push_back(vertex[i]);
  }
}

void coalengine::GeometryBuilder::AddIndex(const unsigned int* index, unsigned int size)
{
  for (unsigned int i = 0; i < size; i++)
  {
    index_.push_back(index[i]);
  }
}

void coalengine::GeometryBuilder::AddNormals(const float* normals, unsigned int size)
{
  for (unsigned int i = 0; i < size; i++)
  {
    normals_.push_back(normals[i]);
  }
}

void coalengine::GeometryBuilder::AddUV(const float* uv, unsigned int size)
{
  for (unsigned int i = 0; i < size; ++i)
  {
    uv_.push_back(uv[i]);
  }
}

void coalengine::GeometryBuilder::SetMaterial(uint32 mat)
{
  if(MaterialBuilder::IsValidMaterial(mat))
  {
    mat_ = RenderList::Instance().material_map_[mat].get();
  }

}

void coalengine::GeometryBuilder::SetMaterial(uint32 geo, uint32 mat)
{
  if(MaterialBuilder::IsValidMaterial(mat))
  {
    if(RenderList::Instance().IsValidGeometry(geo))
    {
      RenderList::Instance().geometry_map_[geo].get()->mat_ =
        RenderList::Instance().material_map_[mat].get();
    }
  }
}

void coalengine::GeometryBuilder::SetShape(Shape shape)
{
  shape_ = shape;
}

uint32 coalengine::GeometryBuilder::GetMaterial(uint32 id)
{
  if (RenderList::Instance().IsValidGeometry(id))
  {
    return  RenderList::Instance().geometry_map_[id]->mat_->id_;
  }
  return 0;
}

glm::mat4 coalengine::GeometryBuilder::GetTransform(uint32 id)
{
  if(RenderList::Instance().IsValidGeometry(id))
  {
    return  RenderList::Instance().geometry_map_[id]->transform_;
  }

  return glm::mat4(0.0f);
}


uint32 coalengine::GeometryBuilder::LoadObj(std::string inputfile)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn;
  std::string err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

  if (!ret)
  {
    printf("Error loading obj!\n");
    return 0;
  }

  std::vector<float> tmp_vertex_;
  std::vector<float> tmp_normals_;
  std::vector<float> tmp_uv_;
  std::vector<unsigned int> tmp_index_;


  // Loop over shapes

  for (auto& shape : shapes) {
    int index = 0;
    for (auto& idx : shape.mesh.indices) {

       tmp_index_.push_back(index);

      tmp_vertex_.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
      tmp_vertex_.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
      tmp_vertex_.push_back(attrib.vertices[3 * idx.vertex_index + 2]);

      tmp_normals_.push_back(attrib.normals[3 * idx.normal_index + 0]);
      tmp_normals_.push_back(attrib.normals[3 * idx.normal_index + 1]);
      tmp_normals_.push_back(attrib.normals[3 * idx.normal_index + 2]);


      if (idx.texcoord_index >= 0) {
          tmp_uv_.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
          tmp_uv_.push_back(1 - (attrib.texcoords[2 * idx.texcoord_index + 1]));
      }

      index++;
    }
  }


  vertices_ = tmp_vertex_;
  normals_ = tmp_normals_;
  uv_ = tmp_uv_;
  index_ = tmp_index_;
  shape_ = Shape::kOther;


  uint32 id = Finish();

  return id;
}


uint32 coalengine::GeometryBuilder::Finish()
{
  px::Mem<Geometry> geometry_;
  geometry_.alloc();
  geometry_->vertices_ = std::move(vertices_);
  geometry_->normals_ = std::move(normals_);
  geometry_->uv_ = std::move(uv_);
  geometry_->index_ = std::move(index_);
  geometry_->mat_ = mat_;
  geometry_->shape_ = shape_;

  ResetBuilder();
  geometry_->uploaded_to_gpu_ = false;
  uint32 id = RenderList::Instance().num_geometries_;
  RenderList::Instance().num_geometries_++;
  id = id | 0x10000000;
  geometry_->id_ = id;
  RenderList::Instance().SubmitGeometry(std::move(geometry_));
  return  id;
}


void coalengine::GeometryBuilder::ResetBuilder()
{
  vertices_.clear();
  normals_.clear();
  uv_.clear();
  index_.clear();
  mat_ = nullptr;
  shape_ = Shape::kOther;
}
