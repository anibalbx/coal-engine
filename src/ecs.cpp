#include "ecs.h"
#include <assert.h>
#include <imgui.h>
#include <ImGuizmo.h>
#include <gtx/transform.hpp>
#include "renderlist.h"
#include <glm/gtx/rotate_vector.hpp>
#include <gtc/type_ptr.hpp>
#include "minitrace.h"

coalengine::ECS& coalengine::ECS::Instance()
{
  static ECS* inst = nullptr;
  if (inst == nullptr) {
    inst = new ECS();
  }
  return *inst;
}

coalengine::ECS::ECS()
{
  num_entities_ = 0;
}



coalengine::ECS::~ECS()
{

}


coalengine::Entity* coalengine::ECS::AllocateEntity()
{
  entities_to_create_.emplace_back();
  entities_to_create_.back().alloc();
  Entity *ent = entities_to_create_.back().get();
  num_entities_++;
  uint32 id = num_entities_ & 0x000FFFFF;
  id = id | 0x40100000;
  id = AddEntityVersion(id);
  ent->id_ = id;
  ent->name_ = "Entity";
  return ent;
}


coalengine::Entity* coalengine::ECS::DeleteEntity(Entity* ent)
{
  if (ent != nullptr && ent->id_ != 0)
  {
    entities_to_delete_.push_back(ent);
    return nullptr;
  }
  return ent;
}

coalengine::WorldTransform* coalengine::ECS::NewWorldTransform(Entity* ent)
{
  assert(ent != nullptr && ent->id_ != 0);
  if (!ent->HasComponents(Entity::WORLD))
  {
    uint32 realid = RealEntityId(ent->id_);
    ent->components_ = ent->components_ | Entity::WORLD;
    ent->id_ = AddEntityVersion(ent->id_);
    ent->newcomp_ = true;
    world_entc_map_[realid].alloc();
    return world_entc_map_[realid].get();
  }
  return nullptr;
}

coalengine::LocalTransform* coalengine::ECS::NewLocalTransform(Entity* ent)
{
  assert(ent != nullptr && ent->id_ != 0);
  if (!ent->HasComponents(Entity::LOCAL))
  {
    uint32 realid = RealEntityId(ent->id_);
    ent->components_ = ent->components_ | Entity::LOCAL;
    ent->id_ = AddEntityVersion(ent->id_);
    ent->newcomp_ = true;
    local_entc_map_[realid].alloc();
    //Check if get World
    NewWorldTransform(ent);

    return local_entc_map_[realid].get();
  }
  return nullptr;
}

coalengine::LocalTransformTRS* coalengine::ECS::NewLocalTransformTRS(Entity* ent)
{
  assert(ent != nullptr && ent->id_ != 0);
  if(!ent->HasComponents(Entity::LOCALTRS))
  {
    uint32 realid = RealEntityId(ent->id_);
    ent->components_ = ent->components_ | Entity::LOCALTRS;
    ent->id_ = AddEntityVersion(ent->id_);
    ent->newcomp_ = true;
    trs_entc_map_[realid].alloc();
    //Check if get World and Local
    NewWorldTransform(ent);
    NewLocalTransform(ent);

    return trs_entc_map_[realid].get();
  }
  return nullptr;
}

coalengine::RenderComponent* coalengine::ECS::NewRenderComponent(Entity* ent)
{
  assert(ent != nullptr && ent->id_ != 0);
  if (!ent->HasComponents(Entity::RENDER))
  {
    uint32 realid = RealEntityId(ent->id_);
    ent->components_ = ent->components_ | Entity::RENDER;
    ent->id_ = AddEntityVersion(ent->id_);
    ent->newcomp_ = true;
    render_entc_map_[realid].alloc();
    return  render_entc_map_[realid].get();
  }
  return nullptr;
}

coalengine::LightComponent* coalengine::ECS::NewLightComponent(Entity* ent)
{
  assert(ent != nullptr && ent->id_ != 0);
  if (!ent->HasComponents(Entity::LIGHT))
  {
    uint32 realid = RealEntityId(ent->id_);
    ent->components_ = ent->components_ | Entity::LIGHT;
    ent->id_ = AddEntityVersion(ent->id_);
    ent->newcomp_ = true;
    light_entc_map_[realid].alloc();
    return  light_entc_map_[realid].get();
  }
  return nullptr;
}

coalengine::WorldTransform* coalengine::ECS::GetWorldTransform(Entity* ent)
{
  if (ent != nullptr && ent->id_ != 0)
  {
    if (ent->HasComponents(Entity::WORLD))
    {
      uint32 realid = RealEntityId(ent->id_);
      if (ent->uploaded_ && !ent->newcomp_)
      {
        return world_ent_map_[realid].get();
      }
      else
      {
        return world_entc_map_[realid].get();
      }

    }
  }
  return nullptr;
}

coalengine::LocalTransform* coalengine::ECS::GetLocalTransform(Entity* ent)
{
  if (ent != nullptr && ent->id_ != 0)
  {
    if (ent->HasComponents(Entity::LOCAL))
    {
      uint32 realid = RealEntityId(ent->id_);
      if (ent->uploaded_ && !ent->newcomp_)
      {
        return local_ent_map_[realid].get();
      }
      else
      {
        return local_entc_map_[realid].get();
      }

    }
  }
  return nullptr;
}

coalengine::LocalTransformTRS* coalengine::ECS::GetLocalTransformTRS(Entity* ent)
{
  //TODO: DO IT FOR ALL
  if (ent != nullptr && ent->id_ != 0)
  {
    if (ent->HasComponents(Entity::LOCALTRS))
    {
      uint32 realid = RealEntityId(ent->id_);
      if (ent->uploaded_ && !ent->newcomp_)
      {
        return trs_ent_map_[realid].get();
      }
      else
      {
        return trs_entc_map_[realid].get();
      }

    }
  }
  return nullptr;
}

coalengine::RenderComponent* coalengine::ECS::GetRenderComponent(Entity* ent)
{
  if (ent != nullptr && ent->id_ != 0)
  {
    if (ent->HasComponents(Entity::RENDER))
    {
      uint32 realid = RealEntityId(ent->id_);
      if (ent->uploaded_ && !ent->newcomp_)
      {
        return render_ent_map_[realid].get();
      }
      else
      {
        return render_entc_map_[realid].get();
      }

    }
  }
  return nullptr;
}

coalengine::LightComponent* coalengine::ECS::GetLightComponent(Entity* ent)
{
  if (ent != nullptr && ent->id_ != 0)
  {
    if (ent->HasComponents(Entity::LIGHT))
    {
      uint32 realid = RealEntityId(ent->id_);
      if (ent->uploaded_ && !ent->newcomp_)
      {
        return light_ent_map_[realid].get();
      }
      else
      {
        return light_entc_map_[realid].get();
      }

    }
  }
  return nullptr;
}

coalengine::Entity* coalengine::ECS::GetEntityByRealndex(uint32 id)
{
  std::map<uint32, px::Mem<Entity>>::iterator it = entities_map_.find(id);
  if (it != entities_map_.end())
  {
    return it->second.get();
  }
  else
  {
    for (int i = 0; i < entities_to_create_.size(); ++i)
    {
      Entity* ent = entities_to_create_.at(i).get();
      if (RealEntityId(ent->id_) == id)
      {
        return  ent;
      }
    }
  }
  return nullptr;
}

coalengine::Entity* coalengine::ECS::GetEntityByIndex(uint32 id)
{
  if (IsEntity(id))
  {
    uint32 realid = RealEntityId(id);
    std::map<uint32, px::Mem<Entity>>::iterator it = entities_map_.find(realid);

    if (it != entities_map_.end())
    {
      return it->second.get();
    }
    else
    {
      for (int i = 0; i < entities_to_create_.size(); ++i)
      {
        Entity* ent = entities_to_create_.at(i).get();
        if (ent->id_ == id)
        {
          return  ent;
        }
      }
    }
  }

  return nullptr;
}
uint32 coalengine::ECS::GetTotalEntities()
{
  return entities_map_.size();
}

void coalengine::ECS::UpdateWorldLocalNoTRSSystem()
{

  for (std::map<uint32, px::Mem<Entity>>::iterator it = entities_map_.begin(); it != entities_map_.end(); it++)
  {
    if (it->second->HasComponents(Entity::WORLD | Entity::LOCAL) && !it->second->HasComponents(Entity::LOCALTRS) 
      && !it->second->newcomp_)
    {

      Entity* parent = local_ent_map_[it->first]->GetParent();

      if (parent != nullptr)
      {
        uint32 realid = RealEntityId(parent->id_);

        world_ent_map_[it->first]->world_ = world_ent_map_[realid]->world_ * local_ent_map_[it->first]->local_;
      }
      else
      {
        world_ent_map_[it->first]->world_ = local_ent_map_[it->first]->local_;
      }

      //Update pos rot scale in world comp
      world_ent_map_[it->first]->position_ = local_ent_map_[it->first]->position_;
      world_ent_map_[it->first]->rotation_ = local_ent_map_[it->first]->rotation_;
      world_ent_map_[it->first]->scale_ = local_ent_map_[it->first]->scale_;

    }
  }
}

void coalengine::ECS::UpdateWorldLocalWithTRSSystem()
{

  for (std::map<uint32, px::Mem<Entity>>::iterator it = entities_map_.begin(); it != entities_map_.end(); it++)
  {
    if (it->second->HasComponents(Entity::WORLD | Entity::LOCAL | Entity::LOCALTRS) && !it->second->newcomp_)
    {
      Entity* parent = local_ent_map_[it->first]->GetParent();

      if (parent != nullptr)
      {
        uint32 realid = RealEntityId(parent->id_);

        world_ent_map_[it->first]->world_ = world_ent_map_[realid]->world_ * local_ent_map_[it->first]->local_;

        float scale[3];
        float rotation[3];
        float position[3];
        glm::vec3 translation;
        float* world_matrix = (float*)glm::value_ptr(world_ent_map_[it->first]->world_);

        ImGuizmo::DecomposeMatrixToComponents(world_matrix, position, rotation, scale);

        //Update pos rot scale in world comp
        world_ent_map_[it->first]->position_ = glm::vec3(position[0], position[1], position[2]);
        world_ent_map_[it->first]->rotation_ = glm::vec3(rotation[0], rotation[1], rotation[2]);
        world_ent_map_[it->first]->scale_ = glm::vec3(scale[0], scale[1], scale[2]);
      }
      else
      {
        world_ent_map_[it->first]->world_ = local_ent_map_[it->first]->local_;

        //Update pos rot scale in world comp
        world_ent_map_[it->first]->position_ = local_ent_map_[it->first]->position_;
        world_ent_map_[it->first]->rotation_ = local_ent_map_[it->first]->rotation_;
        world_ent_map_[it->first]->scale_ = local_ent_map_[it->first]->scale_;
      }



    }
  }
}

void coalengine::ECS::UpdateTRSSystem()
{

  for (std::map<uint32, px::Mem<Entity>>::iterator it = entities_map_.begin(); it != entities_map_.end(); it++)
  {
    
    if (it->second->HasComponents(Entity::WORLD | Entity::LOCAL | Entity::LOCALTRS) && !it->second->newcomp_)
    {
      LocalTransformTRS* trs = GetLocalTransformTRS(it->second.get());

      glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), trs->scale_);

      glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(trs->rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
      glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(trs->rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
      glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(trs->rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));

      glm::mat4 translationMatrix = glm::translate((glm::mat4(1.0f), trs->position_));

      glm::mat4 model = translationMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrixX * scalingMatrix;

      local_ent_map_[it->first]->local_ = model;
      //Update pos rot scale in local comp
      local_ent_map_[it->first]->position_ = trs->position_;
      local_ent_map_[it->first]->rotation_ = trs->rotation_;
      local_ent_map_[it->first]->scale_ = trs->scale_;


    }
  }
}

void coalengine::ECS::UpdateLights()
{

  for (std::map<uint32, px::Mem<Entity>>::iterator it = entities_map_.begin(); it != entities_map_.end(); it++)
  {
    if (it->second->HasComponents(Entity::LIGHT | Entity::WORLD) && !it->second->newcomp_)
    {

      LightComponent* lightcomp = GetLightComponent(it->second.get());
      WorldTransform* wcomp = GetWorldTransform(it->second.get());
      glm::vec3 position = glm::vec3(wcomp->world_[3][0], wcomp->world_[3][1], wcomp->world_[3][2]);
      //TODO: Move to lightcomponent
      float near_plane = 0.0f, far_plane = 200.0f;
      glm::mat4 light_projection;

      if (lightcomp->GetType() == LightComponent::kDirectional)
      {
        light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::vec3 tmp_vec = glm::vec3(0.0f, 0.0f, -100.0f);
        glm::vec3 rotated_vec = glm::rotateX(tmp_vec, glm::radians(wcomp->rotation_.x));
        rotated_vec = glm::rotateY(rotated_vec, glm::radians(wcomp->rotation_.y));
        rotated_vec = glm::rotateZ(rotated_vec, glm::radians(wcomp->rotation_.z));

        lightcomp->SetDirection(rotated_vec);

        lightcomp->light_view_ = glm::lookAt(position, position + lightcomp->GetDirection(), glm::vec3(0.0, 1.0, 0.0));
        lightcomp->light_space_ = light_projection * lightcomp->light_view_;
        lightcomp->model_ = wcomp->world_;
        lightcomp->projection_ = light_projection;
      }
      else if (lightcomp->GetType() == LightComponent::kSpot)
      {

        light_projection = glm::perspective(glm::radians(lightcomp->GetOuterAngle() * 2.0f), 1.0f, 0.1f, 200.0f);
        glm::vec3 tmp_vec = glm::vec3(0.0f, 0.0f, -100.0f);
        glm::vec3 rotated_vec = glm::rotateX(tmp_vec, glm::radians(wcomp->rotation_.x));
        rotated_vec = glm::rotateY(rotated_vec, glm::radians(wcomp->rotation_.y));
        rotated_vec = glm::rotateZ(rotated_vec, glm::radians(wcomp->rotation_.z));

        lightcomp->SetDirection(rotated_vec);

        lightcomp->light_view_ = glm::lookAt(position, position + lightcomp->GetDirection(), glm::vec3(0.0, 1.0, 0.0));
        lightcomp->light_space_ = light_projection * lightcomp->light_view_;
        lightcomp->model_ = wcomp->world_;
        lightcomp->projection_ = light_projection;

      }
      else if (lightcomp->GetType() == LightComponent::kPoint)
      {

      }

      lightcomp->SetPosition(position);

    }
  }

}
void coalengine::ECS::CreateEngineInstances()
{

  //For new create entities
  while (!entities_to_create_.empty())
  {
    px::Mem<Entity>& enti = entities_to_create_.back();
    Entity* ent = enti.get();
    uint32 realid = RealEntityId(ent->id_);
    ent->uploaded_ = true;
    entities_map_.insert(std::make_pair(realid, std::move(enti)));
    entities_to_create_.pop_back();
  }
  //For components
  if (world_entc_map_.size() > 0)
  {
    for (std::map<uint32, px::Mem<WorldTransform>>::iterator it = world_entc_map_.begin(); it != world_entc_map_.end(); it++)
    {
      world_ent_map_.insert(std::make_pair(it->first, std::move(it->second)));
    }
    world_entc_map_.clear();
  }

  if (local_entc_map_.size() > 0)
  {
    for (std::map<uint32, px::Mem<LocalTransform>>::iterator it = local_entc_map_.begin();
      it != local_entc_map_.end(); it++)
    {
      local_ent_map_.insert(std::make_pair(it->first, std::move(it->second)));
    }
    local_entc_map_.clear();
  }

  if (trs_entc_map_.size() > 0)
  {
    for (std::map<uint32, px::Mem<LocalTransformTRS>>::iterator it = trs_entc_map_.begin();
      it != trs_entc_map_.end(); it++)
    {
      trs_ent_map_.insert(std::make_pair(it->first, std::move(it->second)));
    }
    trs_entc_map_.clear();
  }

  if (render_entc_map_.size() > 0)
  {
    for (std::map<uint32, px::Mem<RenderComponent>>::iterator it = render_entc_map_.begin();
      it != render_entc_map_.end(); it++)
    {
      render_ent_map_.insert(std::make_pair(it->first, std::move(it->second)));
    }
    render_entc_map_.clear();
  }

  if (light_entc_map_.size() > 0)
  {
    for (std::map<uint32, px::Mem<LightComponent>>::iterator it = light_entc_map_.begin();
      it != light_entc_map_.end(); it++)
    {
      light_ent_map_.insert(std::make_pair(it->first, std::move(it->second)));
    }
    light_entc_map_.clear();
  }

  //New comp flag down
  for (std::map<uint32, px::Mem<Entity>>::iterator it = entities_map_.begin(); it != entities_map_.end(); it++)
  {
    it->second.get()->newcomp_ = false;
  }


}
void coalengine::ECS::SepareLights()
{
  coalengine::RenderList& RL = coalengine::RenderList::Instance();
  RL.ClearLights();
  uint8_t computed = 0;
  for (auto iterator = ECS::Instance().light_ent_map_.begin(); iterator != ECS::Instance().light_ent_map_.end(); iterator++)
  {
    if (iterator->second.get()->CanCastShadows())
    {
      RL.lights_.light_spaces_.push_back(iterator->second.get()->light_space_);
      if (RL.lights_.light_w_shadows_ < RL.kMaxShadows)
      {
        RL.lights_.light_w_shadows_++;
      }

    }

    switch (iterator->second->GetType())
    {
    case LightComponent::kNone:
      break;
    case LightComponent::kDirectional:

      if (iterator->second->CanCastShadows() && computed < RL.kMaxShadows)
      {
        RL.lights_.lightdirposshadow_.push_back(iterator->second->GetPosition());
        RL.lights_.lightdirdirshadow_.push_back(iterator->second->GetDirection());
        RL.lights_.lightdircolorshadow_.push_back(glm::vec3(iterator->second->GetColor().x / 255.0f, iterator->second->GetColor().y / 255.0f, iterator->second->GetColor().z / 255.0f));
        RL.lights_.lightdirintensityshadow_.push_back(iterator->second->GetIntensity());
        RL.lights_.light_spaces_dir_.push_back(iterator->second->light_space_);
        RL.lights_.shadow_map_dir_.push_back(RL.shadow_fb_[computed].texture_id_);
        computed++;
      }
      else
      {
        RL.lights_.lightdirpos_.push_back(iterator->second->GetPosition());
        RL.lights_.lightdirdir_.push_back(iterator->second->GetDirection());
        RL.lights_.lightdircolor_.push_back(glm::vec3(iterator->second->GetColor()[0] * 0.003921f, iterator->second->GetColor()[1] * 0.003921f, iterator->second->GetColor()[2] * 0.003921f));
        RL.lights_.lightdirintensity_.push_back(iterator->second->GetIntensity());
      }



      break;
    case LightComponent::kSpot:

      if (iterator->second->CanCastShadows() && computed < RL.kMaxShadows)
      {
        RL.lights_.lightspotposshadow_.push_back(iterator->second->GetPosition());
        RL.lights_.lightspotdirshadow_.push_back(iterator->second->GetDirection());
        RL.lights_.lightspotcolorshadow_.push_back(glm::vec3(iterator->second->GetColor()[0] * 0.003921f, iterator->second->GetColor()[1] * 0.003921f, iterator->second->GetColor()[2] * 0.003921f));
        RL.lights_.lightspotinnerangleshadow_.push_back(glm::cos(glm::radians(iterator->second->GetInnerAngle())));
        RL.lights_.lightspotouterangleshadow_.push_back(glm::cos(glm::radians(iterator->second->GetOuterAngle())));
        RL.lights_.lightspotintensityshadow_.push_back(iterator->second->GetIntensity());
        RL.lights_.light_spaces_spot_.push_back(iterator->second->light_space_);
        RL.lights_.shadow_map_spot_.push_back(RL.shadow_fb_[computed].texture_id_);
        computed++;
      }
      else
      {
        RL.lights_.lightspotpos_.push_back(iterator->second->GetPosition());
        RL.lights_.lightspotdir_.push_back(iterator->second->GetDirection());
        RL.lights_.lightspotcolor_.push_back(glm::vec3(iterator->second->GetColor()[0] * 0.003921f, iterator->second->GetColor()[1] * 0.003921f, iterator->second->GetColor()[2] * 0.003921f));
        RL.lights_.lightspotinnerangle_.push_back(glm::cos(glm::radians(iterator->second->GetInnerAngle())));
        RL.lights_.lightspotouterangle_.push_back(glm::cos(glm::radians(iterator->second->GetOuterAngle())));
        RL.lights_.lightspotintensity_.push_back(iterator->second->GetIntensity());
      }

      break;
    case LightComponent::kPoint:

      RL.lights_.lightpointpos_.push_back(iterator->second->GetPosition());
      RL.lights_.lightpointdir_.push_back(iterator->second->GetDirection());
      RL.lights_.lightpointcolor_.push_back(glm::vec3(iterator->second->GetColor()[0] * 0.003921f, iterator->second->GetColor()[1] * 0.003921f, iterator->second->GetColor()[2] * 0.003921f));
      RL.lights_.lightpointintensity_.push_back(iterator->second->GetIntensity());
      break;
    default:
      break;
    }
  }
}

void coalengine::ECS::RenderSystem()
{

  for (std::map<uint32, px::Mem<Entity>>::iterator it = entities_map_.begin(); it != entities_map_.end(); it++)
  {

    if (it->second->HasComponents(Entity::WORLD | Entity::RENDER))
    {
      entities_to_draw_.push_back(it->second.get());
    }
  }

}
/*!!!! Not mutex imp, only execute in a safe thread*/
void coalengine::ECS::DeleteEntitiesSystem()
{
  while (!entities_to_delete_.empty())
  {
    Entity* ent = entities_to_delete_.back();
    uint32 realid = RealEntityId(ent->id_);

    LocalTransform * loc = local_ent_map_[realid].get();


    /*Dereference childs*/
    for (std::map<uint32, Entity*>::iterator it = ent->childs_.begin(); it != ent->childs_.end(); it++)
    {
      uint32 realid_aux = RealEntityId(it->second->id_);

      LocalTransform* loc_c = local_ent_map_[realid_aux].get();
      loc_c->SetParent(nullptr);

    }
    loc->GetParent()->childs_.erase(realid);
    /*Erase components*/
    if (ent->HasComponents(Entity::WORLD))
    {
      world_ent_map_.erase(realid);
    }
    if (ent->HasComponents(Entity::LOCAL))
    {
      local_ent_map_.erase(realid);
    }
    if (ent->HasComponents(Entity::LOCALTRS))
    {
      trs_ent_map_.erase(realid);
    }
    if (ent->HasComponents(Entity::RENDER))
    {
      render_ent_map_.erase(realid);
    }
    if (ent->HasComponents(Entity::LIGHT))
    {
      light_ent_map_.erase(realid);
    }

    entities_map_.erase(realid);
    entities_to_delete_.pop_back();

  }
}
bool coalengine::ECS::IsEntity(uint32 num)
{
  int res = num & 0xF0000000;
  if (res == 0x40000000)
  {
    return true;
  }
  return false;
}
uint32 coalengine::ECS::RealEntityId(uint32 id)
{

  if (IsEntity(id))
  {
    return id & 0x000FFFFF;
  }
  return 0;

}

uint32 coalengine::ECS::AddEntityVersion(uint32 id)
{
  if (IsEntity(id))
  {
    uint32 version = (id & 0x0FF00000) >> 20;
    version = (version + 1) << 20;
    id = (id & 0xF00FFFFF) | version;
    return id;
  }
  return 0;
}
uint32 coalengine::ECS::GetVersionEntity(uint32 id)
{
  if (IsEntity(id))
  {
    return (id & 0x0FF00000) >> 20;
  }
  return 0;
}
