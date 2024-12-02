#ifndef __ECS_H__
#define __ECS_H__ 1

#include <map>
#include <px_mem.h>
#include <vector>
#include "entity.h"
#include "lightcomponent.h"
#include "localtransform.h"
#include "localtransformtrs.h"
#include "rendercomponent.h"
#include "worldtransform.h"


namespace coalengine
{

  class ECS {
  public:
    static ECS& Instance();

    ECS();

    ECS(const ECS& o) = default;

    ~ECS();

    std::vector<Entity*> entities_to_draw_;
    std::vector<Entity*> entities_to_delete_;
    std::vector <px::Mem<Entity>> entities_to_create_;

    // Maps that store the entities and components that are currently in scene.
    std::map<uint32, px::Mem<Entity>> entities_map_;
    std::map<uint32, px::Mem<WorldTransform>> world_ent_map_;
    std::map<uint32, px::Mem<LocalTransform>> local_ent_map_;
    std::map<uint32, px::Mem<LocalTransformTRS>> trs_ent_map_;
    std::map<uint32, px::Mem<RenderComponent>> render_ent_map_;
    std::map<uint32, px::Mem<LightComponent>> light_ent_map_;

    // Maps that store the entities and components that will be created later, at the end of the frame. 
    std::map<uint32, px::Mem<WorldTransform>> world_entc_map_;
    std::map<uint32, px::Mem<LocalTransform>> local_entc_map_;
    std::map<uint32, px::Mem<LocalTransformTRS>> trs_entc_map_;
    std::map<uint32, px::Mem<RenderComponent>> render_entc_map_;
    std::map<uint32, px::Mem<LightComponent>> light_entc_map_;


    Entity* AllocateEntity();

    Entity* DeleteEntity(Entity* ent);

    WorldTransform* NewWorldTransform(Entity* ent);

    LocalTransform* NewLocalTransform(Entity* ent);

    LocalTransformTRS* NewLocalTransformTRS(Entity* ent);

    RenderComponent* NewRenderComponent(Entity* ent);

    LightComponent* NewLightComponent(Entity* ent);

    WorldTransform* GetWorldTransform(Entity* ent);

    LocalTransform* GetLocalTransform(Entity* ent);

    LocalTransformTRS* GetLocalTransformTRS(Entity* ent);

    RenderComponent* GetRenderComponent(Entity* ent);

    LightComponent* GetLightComponent(Entity* ent);

    // Returns the entity by its real index (The engine one).
    Entity* GetEntityByRealndex(uint32 id);

    // Returns the entity by its user index.
    Entity* GetEntityByIndex(uint32 id);

    uint32 GetTotalEntities();


    // System that updates entities with world or local, but no local trs (static objects);
    void UpdateWorldLocalNoTRSSystem();

    // System that updates entities with world or local, and local trs (dynamic objects);
    void UpdateWorldLocalWithTRSSystem();

    //System that updates de entities with trs component.
    void UpdateTRSSystem();

    void UpdateLights();

    // Creates the entities and components that are created on run time.
    void CreateEngineInstances();

    // Classify the light by its type and update its properties.
    void SepareLights();

    // Updates de entities with render component.
    void RenderSystem();

    /*!!!! Not mutex imp, only execute in a safe thread*/
    void DeleteEntitiesSystem();

    // Check if the given id is an entity or not.
    bool IsEntity(uint32 num);

    uint32 num_entities_;

    uint32 RealEntityId(uint32 id);

    // Updates entity version
    uint32 AddEntityVersion(uint32 id);

    uint32 GetVersionEntity(uint32 id);
  };
}

#endif