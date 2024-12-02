#include "engineutils.h"
#include <ext/scalar_constants.hpp>
#include "stdio.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include "math.h"
coalengine::EngineUtils& coalengine::EngineUtils::Instance()
{
  static EngineUtils* inst = nullptr;
  if (inst == nullptr) {
    inst = new EngineUtils();
  }
  return *inst;
}


coalengine::EngineUtils::EngineUtils()
{
  frame_time_ = 0.0;
  timer_.reset();
  width = 0;
  height = 0;
  show_engine_front_ = true;

  entity_id_sel_ = 0;
  light_types.push_back("None");
  light_types.push_back("Directional");
  light_types.push_back("Spot");
  light_types.push_back("Point");

  active_deferred_ = false;

  current_guizmo_mode_ = ImGuizmo::WORLD;
  current_guizmo_opeation_ = ImGuizmo::TRANSLATE;

  play_update_ = false;

}



coalengine::EngineUtils::~EngineUtils()
{}

void coalengine::EngineUtils::StartFrameTimer()
{
  timer_.reset();
}
void coalengine::EngineUtils::EndFrameTimer()
{
  frame_time_ = timer_.elapsed();
}

bool coalengine::EngineUtils::LoadStringFromFile(const char* filepath, std::string& str)
{
  std::ifstream file;

  file.open(filepath); //open the input file

  std::stringstream strStream;

  strStream << file.rdbuf(); //read the file
  str = strStream.str(); //str holds the content of the file

  return true;
}

void coalengine::EngineUtils::AddToLog(std::string text)
{
  coal_log_string_.append("[");
  auto now = std::chrono::system_clock::now();
  auto now_time = std::chrono::system_clock::to_time_t(now);
  auto time_char = std::ctime(&now_time);
  coal_log_string_.append(time_char);
  coal_log_string_ = coal_log_string_.substr(0,coal_log_string_.size() - 1);
  coal_log_string_.append("]: ");


  text.append("\n");
  coal_log_string_.append(text);
  printf(text.c_str());
}

std::string coalengine::EngineUtils::ReturnLog()
{
  return coal_log_string_;
}



void coalengine::EngineUtils::GenerateSphere(int stacks, int sectors, float radius, std::vector<float>& vertices,
  std::vector<float>& normals, std::vector<float>& uv, std::vector<unsigned int>& index)
{

  float lengthInv = 1.0f / radius;    // vertex normal

  float sectorStep = 2 * glm::pi<float>() / sectors;
  float stackStep = PI / stacks;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= stacks; ++i)
  {
    stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
    float xy = radius * cosf(stackAngle);             // r * cos(u)
    float z = radius * sinf(stackAngle);              // r * sin(u)

    // add (sectorCount+1) vertices per stack
    // the first and last vertices have same position and normal, but different tex coords
    for (int j = 0; j <= sectors; ++j)
    {
      sectorAngle = j * sectorStep;           // starting from 0 to 2pi

      // vertex position (x, y, z)
      float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
      float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);

      // normalized vertex normal (nx, ny, nz)
      float nx = x * lengthInv;
      float ny = y * lengthInv;
      float nz = z * lengthInv;

      normals.push_back(nx);
      normals.push_back(ny);
      normals.push_back(nz);

      // vertex tex coord (s, t) range between [0, 1]
      float s = (float)j / sectors;
      float t = (float)i / stacks;

      uv.push_back(s);
      uv.push_back(t);
    }
  }

  for (int i = 0; i < stacks; ++i)
  {
    unsigned int k1 = i * (sectors + 1);     // beginning of current stack
    unsigned int k2 = k1 + sectors + 1;      // beginning of next stack

    for (int j = 0; j < sectors; ++j, ++k1, ++k2)
    {
      // 2 triangles per sector excluding first and last stacks
      // k1 => k2 => k1+1
      if (i != 0)
      {
        index.push_back(k1);
        index.push_back(k2);
        index.push_back(k1 + 1);
      }

      // k1+1 => k2 => k2+1
      if (i != (stacks - 1))
      {
        index.push_back(k1 + 1);
        index.push_back(k2);
        index.push_back(k2 + 1);
      }
    }
  }
}


void coalengine::EngineUtils::GeneratePlane(std::vector<float>& vertex, std::vector<float>& normals,
  std::vector<float>& uv, std::vector<unsigned int>& index)
{
  float vertex_[] =
  {
      -1.0f,-1.0f,0.0f,
      1.0f,-1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f,

  };
  float normals_[] = {

      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f,

  };

  float uv_[] = {

      0.0f,0.0f,
      1.0f,0.0f,
      0.0f,1.0f,
      1.0f,1.0f,

  };

  unsigned int index_[] =
  {
      0, 1, 2,
      1, 3, 2,
  };

  for (unsigned int i = 0; i < 12; ++i)
  {
    vertex.push_back(vertex_[i]);
  }
  for (unsigned int i = 0; i < 12; ++i)
  {
    normals.push_back(normals_[i]);
  }
  for (unsigned int i = 0; i < 8; ++i)
  {
    uv.push_back(uv_[i]);
  }
  for (unsigned int i = 0; i < 6; ++i)
  {
    index.push_back(index_[i]);
  }
}

void coalengine::EngineUtils::GenerateCube(std::vector<float>& vertex, std::vector<float>& normals,
  std::vector<float>& uv, std::vector<unsigned int>& index)
{
  float vertex_c[] =
  {
    // Front Face
    -0.5f, 0.5f,0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    // Right Face
    0.5f, 0.5f,0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    // Left Face    
    -0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    // Back Face    
    0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    // Bottom Face   
    -0.5f, -0.5f,0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    // Top Face
    -0.5f, 0.5f,-0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,

  };

  float normals_c[] =
  {
    // Front Face

    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    // Right Face
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    // Left Face    

    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    // Back Face    

    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    // Bottom Face   
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    // Top Face

    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

  };
  float uv_c[] =
  {
    // Front Face

    0.0f,0.0f,
    1.0f,0.0f,
    1.0f,1.0f,
    0.0f,1.0f,

    // Right Face

    0.0f,0.0f,
    1.0f,0.0f,
    1.0f,1.0f,
    0.0f,1.0f,

    // Left Face    
    0.0f,0.0f,
    1.0f,0.0f,
    1.0f,1.0f,
    0.0f,1.0f,

    // Back Face    
    0.0f,0.0f,
    1.0f,0.0f,
    1.0f,1.0f,
    0.0f,1.0f,

    // Bottom Face   
    0.0f,0.0f,
    1.0f,0.0f,
    1.0f,1.0f,
    0.0f,1.0f,

    // Top Face
    0.0f,0.0f,
    1.0f,0.0f,
    1.0f,1.0f,
    0.0f,1.0f,
  };

  unsigned int index_c[] =
  {
    0, 2, 1, 0, 3, 2,
    4, 6, 5, 4, 7, 6,
    8, 11, 10, 8, 10, 9,
    12, 15, 14, 12 , 14 , 13,
    16, 18 ,17, 16, 19, 18,
    20, 22, 21, 20, 23, 22
  };


  for (unsigned int i = 0; i < sizeof(vertex_c) / sizeof(float); ++i)
  {
    vertex.push_back(vertex_c[i]);
  }

  for (unsigned int i = 0; i < sizeof(normals_c) / sizeof(float); ++i)
  {
    normals.push_back(normals_c[i]);
  }
  for (unsigned int i = 0; i < sizeof(uv_c) / sizeof(float); ++i)
  {
    uv.push_back(uv_c[i]);
  }

  for (unsigned int i = 0; i < sizeof(index_c) / sizeof(unsigned int); ++i)
  {
    index.push_back(index_c[i]);
  }
}

coalengine::BoundingBox coalengine::EngineUtils::GenerateBoundingBox(std::vector<float> vertex)
{
  float
    min_x, max_x,
    min_y, max_y,
    min_z, max_z;

  min_x = max_x = vertex[0];
  min_y = max_y = vertex[1];
  min_z = max_z = vertex[2];


  for (unsigned int i = 0; i < vertex.size() / 3; i++) {
    if (vertex[i * 3] < min_x) min_x = vertex[i * 3];
    if (vertex[i * 3] > max_x) max_x = vertex[i * 3];
    if (vertex[i * 3 + 1] < min_y) min_y = vertex[i * 3 + 1];
    if (vertex[i * 3 + 1] > max_y) max_y = vertex[i * 3 + 1];
    if (vertex[i * 3 + 2] < min_z) min_z = vertex[i * 3 + 2];
    if (vertex[i * 3 + 2] > max_z) max_z = vertex[i * 3 + 2];
  }

  BoundingBox box = BoundingBox(min_x, max_x, min_y, max_y, min_z, max_z);

  box.corners.push_back(glm::vec4(min_x, min_y, min_z, 1.0f));
  box.corners.push_back(glm::vec4(min_x, min_y, max_z, 1.0f));
  box.corners.push_back(glm::vec4(min_x, max_y, min_z, 1.0f));

  box.corners.push_back(glm::vec4(max_x, min_y, min_z, 1.0f));

  box.corners.push_back(glm::vec4(min_x, max_y, max_z, 1.0f));
  box.corners.push_back(glm::vec4(max_x, min_y, max_z, 1.0f));
  box.corners.push_back(glm::vec4(max_x, max_y, min_z, 1.0f));

  box.corners.push_back(glm::vec4(max_x, max_y, max_z, 1.0f));

  return box;
}

