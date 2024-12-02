#ifndef __COALENGINELUA_H__
#define __COALENGINELUA_H__ 1

#include <px_mem.h>
#include <vec3.hpp>
#include <vector>
#include "ecs.h"
#include "engineutils.h"
#include "texture.h"
#include "geometrybuilder.h"
#include "material.h"
#include "materialbuilder.h"
#include "texturebuilder.h"
#include "entity.h"
#include "renderlist.h"
#include "glm/mat4x4.hpp"

extern "C" {
#include "lualib.h"
#include "lauxlib.h"
#include "lua.h"
}


namespace coalengine
{
  class CoalEngineLua
  {
  public:

    static CoalEngineLua& Instance()
    {
      static CoalEngineLua* inst = nullptr;
      if (inst == nullptr) {
        inst = new CoalEngineLua();
      }
      return *inst;
    }

    lua_State* ctx_; /**< Lua Context */

    CoalEngineLua()
    {
      ctx_ = luaL_newstate();
      luaL_openlibs(ctx_);


      lua_register(ctx_, "CreateDiffuseMaterial", CreateDiffuseMaterial);
      lua_register(ctx_, "CreatePBRMaterial", CreatePBRMaterial);
      lua_register(ctx_, "CreateCube", CreateCube);
      lua_register(ctx_, "CreateSphere", CreateSphere);
      lua_register(ctx_, "CreatePlane", CreatePlane);
      lua_register(ctx_, "CreateObj", CreateObj);

      lua_register(ctx_, "CreateTexture", CreateTexture);
      lua_register(ctx_, "CreateEntity", CreateEntity);
      lua_register(ctx_, "NewWorldTransform", NewWorldTransform);
      lua_register(ctx_, "NewLocalTransform", NewLocalTransform);
      lua_register(ctx_, "NewLocalTransformTRS", NewLocalTransformTRS);
      lua_register(ctx_, "NewRenderComponent", NewRenderComponent);

      lua_register(ctx_, "SetMesh", SetMesh);
      lua_register(ctx_, "SetWorld", SetWorld);
      lua_register(ctx_, "SetLocal", SetLocal);
      lua_register(ctx_, "SetPositionTRS", SetPositionTRS);
      lua_register(ctx_, "SetScaleTRS", SetScaleTRS);
      lua_register(ctx_, "SetRotationTRS", SetRotationTRS);
      lua_register(ctx_, "SetChild", SetChild);
      lua_register(ctx_, "SetName", SetName);
      lua_register(ctx_, "DeleteEntity", DeleteEntity);

      lua_register(ctx_, "SetColor", SetColor);
      lua_register(ctx_, "SetEmissive", SetEmissive);
      lua_register(ctx_, "SetTexture", SetTexture);
      lua_register(ctx_, "SetMaterial", SetMaterial);

      //pbr
      lua_register(ctx_, "SetNormalMap", SetNormalMap);
      lua_register(ctx_, "SetMetallicMap", SetMetallicMap);
      lua_register(ctx_, "SetRoughnessMap", SetRoughnessMap);
      lua_register(ctx_, "SetAOMap", SetAOMap);

      //Lights

      lua_register(ctx_, "NewDirectionalLightComponent", NewDirectionalLightComponent);
      lua_register(ctx_, "NewSpotLightComponent", NewSpotLightComponent);
      lua_register(ctx_, "NewPointLightComponent", NewPointLightComponent);


      lua_register(ctx_, "SetLightDirection", SetLightDirection);
      lua_register(ctx_, "SetLightInnerAngle", SetLightInnerAngle);
      lua_register(ctx_, "SetLightOuterAngle", SetLightOuterAngle);
      lua_register(ctx_, "SetLightCastShadows", SetLightCastShadows);
      lua_register(ctx_, "SetLightColor", SetLightColor);
      lua_register(ctx_, "SetLightIntensity", SetLightIntensity);

      lua_register(ctx_, "GetColor", GetColor);
      lua_register(ctx_, "GetPosition", GetPosition);
      lua_register(ctx_, "GetScale", GetScale);
      lua_register(ctx_, "GetRotation", GetRotation);
      lua_register(ctx_, "GetTexture", GetTexture);
      lua_register(ctx_, "GetMaterial", GetMaterial);
    }



    CoalEngineLua(const CoalEngineLua& o) = default;


    ~CoalEngineLua()
    {
      lua_close(ctx_);
    }


    static int CreateDiffuseMaterial(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      MB->CreateMaterial(Material::Type::kMaterial_Diffuse);
      uint32 id = MB->Finish();
      lua_pushinteger(ctx, id);
      return 1;
    }

    static int CreatePBRMaterial(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      MB->CreateMaterial(Material::Type::kMaterial_PBR);
      uint32 id = MB->Finish();
      lua_pushinteger(ctx, id);
      return 1;
    }


    static int CreateTexture(lua_State* ctx)
    {
      px::Mem<TextureBuilder> TB;
      TB.alloc();
      std::string text_s = lua_tostring(ctx, 1);
      uint32 id = TB->CreateTexture(text_s.c_str());
      lua_pushinteger(ctx, id);
      return 1;
    }


    static int SetTexture(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      uint32 text_id = (uint32)lua_tointeger(ctx, 1);
      uint32 mat_id = (uint32)lua_tointeger(ctx, 2);
      MB->SetTexture(mat_id, text_id);
      return 1;
    }


    static int SetColor(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      uint32 mat_id = (uint32)lua_tointeger(ctx, 1);
      float color[4];

      lua_rawgeti(ctx, 2, 1);
      color[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 2);
      color[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 3);
      color[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 4);
      color[3] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);


      MB->SetColor(mat_id, glm::vec4(color[0], color[1], color[2], color[3]));
      return 1;
    }

    static int SetEmissive(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      uint32 mat_id = (uint32)lua_tointeger(ctx, 1);
      float color[4];

      bool can = (float)lua_toboolean(ctx, 2);


      MB->SetEmissive(mat_id, can);
      return 1;
    }

    static int SetNormalMap(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      uint32 text_id = (uint32)lua_tointeger(ctx, 1);
      uint32 mat_id = (uint32)lua_tointeger(ctx, 2);
      MB->SetNormalMap(mat_id, text_id);
      return 1;
    }

    static int SetMetallicMap(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      uint32 text_id = (uint32)lua_tointeger(ctx, 1);
      uint32 mat_id = (uint32)lua_tointeger(ctx, 2);
      MB->SetMetallicMap(mat_id, text_id);
      return 1;
    }

    static int SetRoughnessMap(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      uint32 text_id = (uint32)lua_tointeger(ctx, 1);
      uint32 mat_id = (uint32)lua_tointeger(ctx, 2);
      MB->SetRoughnessMap(mat_id, text_id);
      return 1;
    }
    static int SetAOMap(lua_State* ctx)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      uint32 text_id = (uint32)lua_tointeger(ctx, 1);
      uint32 mat_id = (uint32)lua_tointeger(ctx, 2);
      MB->SetAOMap(mat_id, text_id);
      return 1;
    }

    static int CoalEngineLua::CreateCube(lua_State* ctx)
    {
      std::vector<float> vertex;
      std::vector<float> uv;
      std::vector<float> normal;
      std::vector<unsigned int> index;

      px::Mem<GeometryBuilder> GB;
      GB.alloc();

      EngineUtils::Instance().GenerateCube(vertex, normal, uv, index);

      GB->AddVertex(vertex.data(), vertex.size());
      GB->AddNormals(normal.data(), normal.size());
      GB->AddUV(uv.data(), uv.size());
      GB->AddIndex(index.data(), index.size());
      GB->SetShape(Shape::kCube);

      uint32 geometry_id = GB->Finish();
      lua_pushinteger(ctx, geometry_id);
      return 1;
    }

    static int CoalEngineLua::CreatePlane(lua_State* ctx)
    {
      std::vector<float> vertex;
      std::vector<float> uv;
      std::vector<float> normal;
      std::vector<unsigned int> index;

      px::Mem<GeometryBuilder> GB;
      GB.alloc();

      EngineUtils::Instance().GeneratePlane(vertex, normal, uv, index);

      GB->AddVertex(vertex.data(), vertex.size());
      GB->AddNormals(normal.data(), normal.size());
      GB->AddUV(uv.data(), uv.size());
      GB->AddIndex(index.data(), index.size());
      GB->SetShape(Shape::kPlane);

      uint32 geometry_id = GB->Finish();
      lua_pushinteger(ctx, geometry_id);
      return 1;
    }

    static int CoalEngineLua::CreateSphere(lua_State* ctx)
    {
      std::vector<float> vertex;
      std::vector<float> uv;
      std::vector<float> normal;
      std::vector<unsigned int> index;

      px::Mem<GeometryBuilder> GB;
      GB.alloc();

      EngineUtils::Instance().GenerateSphere(64, 64, 1.0f, vertex, normal, uv, index);

      GB->AddVertex(vertex.data(), vertex.size());
      GB->AddNormals(normal.data(), normal.size());
      GB->AddUV(uv.data(), uv.size());
      GB->AddIndex(index.data(), index.size());
      GB->SetShape(Shape::kSphere);

      uint32 geometry_id = GB->Finish();
      lua_pushinteger(ctx, geometry_id);
      return 1;
    }


    static int CoalEngineLua::CreateObj(lua_State* ctx)
    {

      std::string text_s = lua_tostring(ctx, 1);
      px::Mem<GeometryBuilder> GB;
      GB.alloc();
      uint32 geometry_id = GB->LoadObj(text_s);
      lua_pushinteger(ctx, geometry_id);
      return 1;
    }

    static int CoalEngineLua::CreateEntity(lua_State* ctx)
    {
      uint32 id = 0;
      Entity* ent = ECS::Instance().AllocateEntity();
      if (ent != nullptr)
      {
        id = ECS::Instance().RealEntityId(ent->id_);

      }
      lua_pushinteger(ctx, id);
      return 1;

    }

    // Creates a new world transform component
    static int CoalEngineLua::NewWorldTransform(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);
      ECS::Instance().NewWorldTransform(ent);
      return 1;

    }

    // Creates a new local transform component
    static int CoalEngineLua::NewLocalTransform(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);
      ECS::Instance().NewLocalTransform(ent);
      return 1;

    }

    // Creates a new trs component
    static int CoalEngineLua::NewLocalTransformTRS(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);
      ECS::Instance().NewLocalTransformTRS(ent);
      return 1;

    }

    // Creates a new render component
    static int CoalEngineLua::NewRenderComponent(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);
      ECS::Instance().NewRenderComponent(ent);
      return 1;

    }

    // Creates a new directional light component
    static int CoalEngineLua::NewDirectionalLightComponent(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);
      LightComponent* lt = ECS::Instance().NewLightComponent(ent);
      lt->SetType(LightComponent::kDirectional);
      return 1;

    }

    // Creates a new spot light component
    static int CoalEngineLua::NewSpotLightComponent(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);
      LightComponent* lt = ECS::Instance().NewLightComponent(ent);
      lt->SetType(LightComponent::kSpot);
      return 1;

    }

    // Creates a new point light component
    static int CoalEngineLua::NewPointLightComponent(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);
      LightComponent* lt = ECS::Instance().NewLightComponent(ent);
      lt->SetType(LightComponent::kPoint);
      return 1;

    }

    static int CoalEngineLua::SetLightDirection(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      glm::vec3 light_direction;

      lua_rawgeti(ctx, 2, 1);
      light_direction.x = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 2);
      light_direction.y = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 3);
      light_direction.z = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);

      LightComponent* lt_c = ECS::Instance().GetLightComponent(ent);
      if (ent != nullptr)
      {
        lt_c->SetDirection(light_direction);
      }

      return 1;

    }

    static int CoalEngineLua::SetLightInnerAngle(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      float inner_angle = (float)lua_tonumber(ctx, 2);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      LightComponent* lt_c = ECS::Instance().GetLightComponent(ent);
      if (ent != nullptr)
      {
        lt_c->SetInnerAngle(inner_angle);
      }

      return 1;

    }
    static int CoalEngineLua::SetLightOuterAngle(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      float outer_angle = (float)lua_tonumber(ctx, 2);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      LightComponent* lt_c = ECS::Instance().GetLightComponent(ent);
      if (ent != nullptr)
      {
        lt_c->SetOuterAngle(outer_angle);
      }

      return 1;

    }

    static int CoalEngineLua::SetLightCastShadows(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      bool can = (float)lua_toboolean(ctx, 2);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      LightComponent* lt_c = ECS::Instance().GetLightComponent(ent);
      if (ent != nullptr)
      {
        lt_c->SetCanCastShadows(can);

      }

      return 1;

    }

    static int CoalEngineLua::SetLightColor(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      glm::vec4 color;

      lua_rawgeti(ctx, 2, 1);
      color.x = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 2);
      color.y = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 3);
      color.z = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 4);
      color.w = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);

      LightComponent* lt_c = ECS::Instance().GetLightComponent(ent);
      if (ent != nullptr)
      {
        lt_c->SetColor(color);
      }

      return 1;

    }

    static int CoalEngineLua::SetLightIntensity(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      float intensity = (float)lua_tonumber(ctx, 2);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      LightComponent* lt_c = ECS::Instance().GetLightComponent(ent);
      if (ent != nullptr)
      {
        lt_c->SetIntensity(intensity);
      }

      return 1;

    }

    static int CoalEngineLua::SetMesh(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      uint32 geo_id = (uint32)lua_tointeger(ctx, 2);
      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);
      RenderComponent* render = ECS::Instance().GetRenderComponent(ent);

     /* if (RenderList::Instance().IsValidGeometry(geo_id))
      {
        Geometry* geo = RenderList::Instance().geometry_map_[geo_id].get();
        switch (geo->shape_)
        {
        case Shape::kCube: ent->name_ = "Cube";
          break;
        case Shape::kSphere: ent->name_ = "Sphere";
          break;
        case Shape::kPlane: ent->name_ = "Plane";
          break;
        case Shape::kOther: ent->name_ = "Other";
          break;
        default: ;
        }
      }*/

      if (render != nullptr)
      {
        render->SetMesh(geo_id);
      }
      return 1;

    }

    static int SetPositionTRS(lua_State* ctx)
    {

      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      float pos[3];
      lua_rawgeti(ctx, 2, 1);
      pos[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 2);
      pos[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 3);
      pos[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);

      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      if (ent != nullptr)
      {
        LocalTransformTRS* trs = ECS::Instance().GetLocalTransformTRS(ent);
        if (trs != nullptr)
        {
          trs->SetPosition(pos[0], pos[1], pos[2]);
        }
      }
      return 1;
    }

    static int SetScaleTRS(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      float scale[3];
      lua_rawgeti(ctx, 2, 1);
      scale[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 2);
      scale[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 3);
      scale[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);

      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      if (ent != nullptr)
      {
        LocalTransformTRS* trs = ECS::Instance().GetLocalTransformTRS(ent);
        if (trs != nullptr)
        {
          trs->SetScale(scale[0], scale[1], scale[2]);
        }
      }
      return 1;
    }

    static int SetRotationTRS(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);
      float rot[3];
      lua_rawgeti(ctx, 2, 1);
      rot[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 2);
      rot[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 3);
      rot[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);

      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      if (ent != nullptr)
      {
        LocalTransformTRS* trs = ECS::Instance().GetLocalTransformTRS(ent);
        if (trs != nullptr)
        {
          trs->SetRotation(rot[0], rot[1], rot[2]);
        }
      }
      return 1;
    }

    static int SetWorld(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);

      float pos[3];
      lua_rawgeti(ctx, 2, 1);
      pos[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 2);
      pos[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 3);
      pos[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);


      float rot[3];
      lua_rawgeti(ctx, 3, 1);
      rot[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 3, 2);
      rot[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 3, 3);
      rot[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);

      float scale[3];
      lua_rawgeti(ctx, 4, 1);
      scale[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 4, 2);
      scale[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 4, 3);
      scale[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);


      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      if (ent != nullptr)
      {
        WorldTransform* world = ECS::Instance().GetWorldTransform(ent);
        if (world != nullptr)
        {
          world->SetWorld(glm::vec3(pos[0], pos[1], pos[2]),
            glm::vec3(rot[0], rot[1], rot[2]),
            glm::vec3(scale[0], scale[1], scale[2]));
        }
      }
      return 1;
    }

    static int SetLocal(lua_State* ctx)
    {
      uint32 entity_id = (uint32)lua_tointeger(ctx, 1);

      float pos[3];
      lua_rawgeti(ctx, 2, 1);
      pos[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 2);
      pos[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 2, 3);
      pos[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);


      float rot[3];
      lua_rawgeti(ctx, 3, 1);
      rot[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 3, 2);
      rot[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 3, 3);
      rot[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);

      float scale[3];
      lua_rawgeti(ctx, 4, 1);
      scale[0] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 4, 2);
      scale[1] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);
      lua_rawgeti(ctx, 4, 3);
      scale[2] = (float)lua_tonumber(ctx, -1);
      lua_pop(ctx, 1);


      Entity* ent = ECS::Instance().GetEntityByRealndex(entity_id);

      if (ent != nullptr)
      {
        LocalTransform* local = ECS::Instance().GetLocalTransform(ent);
        if (local != nullptr)
        {

          local->SetLocal(glm::vec3(pos[0], pos[1], pos[2]),
            glm::vec3(rot[0], rot[1], rot[2]),
            glm::vec3(scale[0], scale[1], scale[2]));
        }
      }
      return 1;
    }

    static int SetMaterial(lua_State* ctx)
    {
      uint32 geo = (uint32)lua_tointeger(ctx, 1);
      uint32 mat = (uint32)lua_tointeger(ctx, 2);
      px::Mem<GeometryBuilder> GB;
      GB.alloc();
      GB->SetMaterial(geo, mat);
      return 1;
    }

    static int SetChild(lua_State* ctx)
    {
      uint32 id_c = (uint32)lua_tointeger(ctx, 1);
      uint32 id_p = (uint32)lua_tointeger(ctx, 2);

      Entity* children = ECS::Instance().GetEntityByRealndex(id_c);
      Entity* parent = ECS::Instance().GetEntityByRealndex(id_p);

      parent->SetChildren(children);

      return 1;
    }

    static int SetName(lua_State* ctx)
    {
      std::string text_s = lua_tostring(ctx, 1);
      uint32 id = (uint32)lua_tointeger(ctx, 2);

      Entity* ent = ECS::Instance().GetEntityByRealndex(id);

      ent->SetName(text_s);

      return 1;
    }

    static int DeleteEntity(lua_State* ctx)
    {
      uint32 id = (uint32)lua_tointeger(ctx, 1);
      Entity* entity = ECS::Instance().GetEntityByRealndex(id);
      ECS::Instance().DeleteEntity(entity);

      lua_pushinteger(ctx, 0);

      return 1;
    }

    static int GetColor(lua_State* ctx)
    {
      uint32 mat = (uint32)lua_tointeger(ctx, 1);

      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      glm::vec4 color = MB->GetColor(mat);

      lua_createtable(ctx, 4, 0);//+1

      lua_pushinteger(ctx, (int)color.x);//+1
      lua_rawseti(ctx, -2, 1);//-1

      lua_pushinteger(ctx, (int)color.y);//+1
      lua_rawseti(ctx, -2, 2);//-1

      lua_pushinteger(ctx, (int)color.z);//+1
      lua_rawseti(ctx, -2, 3);//-1

      lua_pushinteger(ctx, (int)color.w);//+1
      lua_rawseti(ctx, -2, 4);//-1

      return 1;
    }

    static int GetPosition(lua_State* ctx)
    {

      uint32 ent_id = (uint32)lua_tointeger(ctx, 1);

      Entity* ent = ECS::Instance().GetEntityByRealndex(ent_id);
      WorldTransform* wt = ECS::Instance().GetWorldTransform(ent);
      //wt->world_
      float position[3];
      position[0] = wt->world_[3][0];
      position[1] = wt->world_[3][1];
      position[2] = wt->world_[3][2];

      lua_createtable(ctx, 3, 0);//+1

      lua_pushnumber(ctx, position[0]);//+1
      lua_rawseti(ctx, -2, 1);//-1

      lua_pushnumber(ctx, position[1]);//+1
      lua_rawseti(ctx, -2, 2);//-1

      lua_pushnumber(ctx, position[2]);//+1
      lua_rawseti(ctx, -2, 3);//-1

      return 1;
    }

    static int GetRotation(lua_State* ctx)
    {
      uint32 ent_id = (uint32)lua_tointeger(ctx, 1);

      Entity* ent = ECS::Instance().GetEntityByRealndex(ent_id);

      glm::vec3 res;

      if (ent->HasComponents(Entity::LOCALTRS))
      {
        LocalTransformTRS* ltrs = ECS::Instance().GetLocalTransformTRS(ent);
        res.x = ltrs->rotation_.x;
        res.y = ltrs->rotation_.y;
        res.z = ltrs->rotation_.z;
      }
      else if (ent->HasComponents(Entity::LOCAL))
      {
        LocalTransform* ltr = ECS::Instance().GetLocalTransform(ent);
        res.x = ltr->rotation_.x;
        res.y = ltr->rotation_.y;
        res.z = ltr->rotation_.z;
      }
      else if (ent->HasComponents(Entity::WORLD))
      {
        WorldTransform* wt = ECS::Instance().GetWorldTransform(ent);
        res.x = wt->rotation_.x;
        res.y = wt->rotation_.y;
        res.z = wt->rotation_.z;
      }

      lua_createtable(ctx, 3, 0);//+1

      lua_pushnumber(ctx, res.x);//+1
      lua_rawseti(ctx, -2, 1);//-1

      lua_pushnumber(ctx, res.y);//+1
      lua_rawseti(ctx, -2, 2);//-1

      lua_pushnumber(ctx, res.z);//+1
      lua_rawseti(ctx, -2, 3);//-1


      return 1;
    }

    static int GetScale(lua_State* ctx)
    {
      uint32 ent_id = (uint32)lua_tointeger(ctx, 1);

      Entity* ent = ECS::Instance().GetEntityByRealndex(ent_id);

      glm::vec3 res;

      if (ent->HasComponents(Entity::LOCALTRS))
      {
        LocalTransformTRS* ltrs = ECS::Instance().GetLocalTransformTRS(ent);
        res.x = ltrs->scale_.x;
        res.y = ltrs->scale_.y;
        res.z = ltrs->scale_.z;
      }
      else if (ent->HasComponents(Entity::LOCAL))
      {
        LocalTransform* ltr = ECS::Instance().GetLocalTransform(ent);
        res.x = ltr->scale_.x;
        res.y = ltr->scale_.y;
        res.z = ltr->scale_.z;
      }
      else if (ent->HasComponents(Entity::WORLD))
      {
        WorldTransform* wt = ECS::Instance().GetWorldTransform(ent);
        res.x = wt->scale_.x;
        res.y = wt->scale_.y;
        res.z = wt->scale_.z;
      }

      lua_createtable(ctx, 3, 0);//+1

      lua_pushnumber(ctx, res.x);//+1
      lua_rawseti(ctx, -2, 1);//-1

      lua_pushnumber(ctx, res.y);//+1
      lua_rawseti(ctx, -2, 2);//-1

      lua_pushnumber(ctx, res.z);//+1
      lua_rawseti(ctx, -2, 3);//-1

      return 1;
    }

    static int GetMaterial(lua_State* ctx)
    {
      uint32 geo = (uint32)lua_tointeger(ctx, 1);

      px::Mem<GeometryBuilder> GB;
      GB.alloc();

      uint32 mat = GB->GetMaterial(geo);
      lua_pushinteger(ctx, mat);

      return 1;
    }

    static int GetTexture(lua_State* ctx)
    {
      uint32 mat = (uint32)lua_tointeger(ctx, 1);

      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      uint32 texture = MB->GetTexture(mat);
      lua_pushinteger(ctx, texture);

      return 1;
    }

    bool CoalEngineLua::RunScript(const char* name)
    {
      if (luaL_loadfile(ctx_, name) != LUA_OK)
      {
        printf("[LUA ERROR] %s\n", lua_tostring(ctx_, -1));
        return false;
      }
      if (lua_pcall(ctx_, 0, 0, 0) == LUA_OK)
      {
        return true;
      }
      printf("[LUA ERROR] %s\n", lua_tostring(ctx_, -1));
      return false;
    }
  };
}

#endif
