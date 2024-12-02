#define JSON_MEASURE 1
#include "drawimgui.h"
#include <cstdio>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <ImGuizmo.h>
#include <string>
#include "coalimgui.h"
#include "engineutils.h"
#include "RenderList.h"
#include <vector>
#include <vec3.hpp>
#include <gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <gtc/type_ptr.hpp>
#include "ecs.h"
#include "minitrace.h"


coalengine::DrawImgui::DrawImgui()
{
  dockspace_id_ = 0;

}

coalengine::DrawImgui::~DrawImgui()
{
}


// Function pointer to pass to the wraper list box
static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
  auto& vector = *static_cast<std::vector<std::string>*>(vec);
  if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
  *out_text = vector.at(idx).c_str();
  return true;
};


bool coalengine::DrawImgui::ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
{
  if (values.empty())
  {
    return false;
  }

  return ImGui::ListBox(label, currIndex, vector_getter, static_cast<void*>(&values), values.size(),20);
}

void coalengine::DrawImgui::PrintTRS(float position[3], float rotation[3], float scale[3], bool editable)
{

  

  ImGuiInputTextFlags_ flags;
  if (editable)
  {
    flags = ImGuiInputTextFlags_None;
  }
  else
  {
    flags = ImGuiInputTextFlags_ReadOnly;
  }

  ImGui::Text("Position");
  //Position X
  ImGui::ColorButton("", ImVec4(1.0, 0.0, 0.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("X");
  ImGui::SameLine();
  ImGui::InputFloat("Position X", &position[0], 0.1f, 0.5f, "%.3f", flags);

  //Position Y
  ImGui::ColorButton("", ImVec4(0.0, 1.0, 0.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("Y");
  ImGui::SameLine();
  ImGui::InputFloat("Position Y", &position[1], 0.1f, 0.5f, "%.3f", flags);

  //Position Z
  ImGui::ColorButton("", ImVec4(0.0, 0.0, 1.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("Z");
  ImGui::SameLine();
  ImGui::InputFloat("Position Z", &position[2], 0.1f, 0.5f, "%.3f", flags);

  ImGui::Text("Rotation");

  //Rotation X
  ImGui::ColorButton("", ImVec4(1.0, 0.0, 0.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("X");
  ImGui::SameLine();
  ImGui::InputFloat("Rotation X", &rotation[0], 0.1f, 0.5f, "%.3f", flags);

  //Rotation Y
  ImGui::ColorButton("", ImVec4(0.0, 1.0, 0.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("Y");
  ImGui::SameLine();
  ImGui::InputFloat("Rotation Y", &rotation[1], 0.1f, 0.5f, "%.3f", flags);

  //Rotation Z
  ImGui::ColorButton("", ImVec4(0.0, 0.0, 1.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("Z");
  ImGui::SameLine();
  ImGui::InputFloat("Rotation Z", &rotation[2], 0.1f, 0.5f, "%.3f", flags);

  ImGui::Text("Scale");

  //Scale X
  ImGui::ColorButton("", ImVec4(1.0, 0.0, 0.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("X");
  ImGui::SameLine();
  ImGui::InputFloat("Scale X", &scale[0], 0.1f, 0.5f, "%.3f", flags);

  //Scale Y
  ImGui::ColorButton("", ImVec4(0.0, 1.0, 0.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("Y");
  ImGui::SameLine();
  ImGui::InputFloat("Scale Y", &scale[1], 0.1f, 0.5f, "%.3f", flags);

  //Scale Z
  ImGui::ColorButton("", ImVec4(0.0, 0.0, 1.0, 1.0));
  ImGui::SameLine();
  ImGui::Text("Z");
  ImGui::SameLine();
  ImGui::InputFloat("Scale Z", &scale[2], 0.1f, 0.5f, "%.3f", flags);

}


void coalengine::DrawImgui::run()
{
#ifdef JSON_MEASURE
  MTR_BEGIN("third", "Imgui operations");
#endif

  EngineUtils* eu = &EngineUtils::Instance();
  ECS& ecs = ECS::Instance();
  RenderList& render = RenderList::Instance();

  render.coal_imgui->ImguiNewFrame();

  ImGuiWindowFlags w_flags;

  if(eu->play_update_)
  {
    w_flags = ImGuiWindowFlags_NoInputs;
  }else
  {
    w_flags = ImGuiWindowFlags_None;
  }

  //Central Dock window
  ImGui::Begin("Coal Engine v0.69", 0, ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
  ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetWindowSize(ImVec2(eu->width, eu->height));
  dockspace_id_ = ImGui::GetID("HUB_DockSpace");
  ImGui::DockSpace(dockspace_id_, ImVec2(0.0f, 0.0f),
    ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::End();

  //Engine features window
  ImGui::SetNextWindowDockID(dockspace_id_, ImGuiCond_FirstUseEver);
  ImGui::Begin("Engine");
  std::string frame_string = "Frame ms: ";
  frame_string += std::to_string(eu->frame_time_);
  frame_string += " FPS: ";
  frame_string += std::to_string(ImGui::GetIO().Framerate);

  // List of entities
  ImGui::Text(frame_string.c_str());
  ImGui::Text("List of Entities:");

  eu->entities_ptr_.clear();
  eu->id_vector_.clear();

  eu->entities_ptr_.push_back(nullptr);
  eu->id_vector_.push_back("None");

  for (std::map<uint32, px::Mem<Entity>>::iterator it = ECS::Instance().entities_map_.begin(); it != ECS::Instance().entities_map_.end(); it++)
  {

    eu->id_vector_.push_back(it->second->name_);
    eu->entities_ptr_.push_back(it->second.get());
  }

  ListBox("Entities", &eu->entity_id_sel_, eu->id_vector_);

  // Toggle Bloom

  bool bloom_before = render.camera_->bloom_active_;

  if (ImGui::Checkbox("Enable bloom", &bloom_before))
  {
    if(bloom_before)
    {
      render.camera_->EnableBloom();
    }else
    {
      render.camera_->DisableBloom();

    }
    
  }

  ImGui::Spacing();


  //Play button
  if(!eu->play_update_)
  {
    if(ImGui::Button("Play"))
    {
      eu->play_update_ = true;
    }
  }else
  {
    if (ImGui::Button("Pause"))
    {
      eu->play_update_ = false;
    }
  }

  ImGui::End();

  //Log Window
  ImGui::SetNextWindowDockID(dockspace_id_, ImGuiCond_FirstUseEver);
  ImGui::Begin("Log");

  ImGui::TextUnformatted(EngineUtils::Instance().ReturnLog().c_str());

  ImGui::End();


  //Properties window
  ImGui::SetNextWindowDockID(dockspace_id_, ImGuiCond_FirstUseEver);
  ImGui::Begin("Properties", 0, w_flags);

  Entity* ent = nullptr;
  WorldTransform*  wt = nullptr;
  LocalTransform* lt = nullptr;
  LocalTransformTRS*  trs = nullptr;
  LightComponent* lc = nullptr;
  RenderComponent* rc = nullptr;

  if (eu->entity_id_sel_ != 0)
  {
    std::string name_ent = "Entity: " + std::to_string(eu->entity_id_sel_);

    
    //Components
    ent = ecs.entities_map_[eu->entity_id_sel_].get();
    wt = ecs.GetWorldTransform(ent);
    lt = ecs.GetLocalTransform(ent);
    trs = ecs.GetLocalTransformTRS(ent);
    lc = ecs.GetLightComponent(ent);
    rc = ecs.GetRenderComponent(ent);

    std::string tmp_name = ent->name_;
    ImGui::InputText("Name", &tmp_name);

    ent->SetName(tmp_name);

    if (lc != nullptr)
    {
      ImGui::Text("Light");
      int type = lc->GetType();
      if (ImGui::Combo("Light type", &type, vector_getter, static_cast<void*>(&eu->light_types), eu->light_types.size()))
      {
        if ((LightComponent::LightType)type == LightComponent::kPoint)
        {
          lc->SetCanCastShadows(false);
        }
        lc->SetType((LightComponent::LightType)type);
      }

      float color[4] = { lc->GetColor().x / 255.0f ,lc->GetColor().y / 255.0f , lc->GetColor().z / 255.0f , lc->GetColor().w / 255.0f };
      if (ImGui::ColorPicker4("Light color", color, ImGuiColorEditFlags_HDR))
      {
        lc->SetColor(glm::vec4(color[0] * 255.0f, color[1] * 255.0f, color[2] * 255.0f, color[3] * 255.0f));

      }

      float intensity = lc->GetIntensity();
      ImGui::InputFloat("Intensity", &intensity, 1.0f, 5.0f);
      lc->SetIntensity(intensity);

      if (lc->GetType() == LightComponent::kSpot)
      {
        float inner_angle = lc->GetInnerAngle();
        float outer_angle = lc->GetOuterAngle();


        ImGui::InputFloat("Inner Angle", &inner_angle, 1.0f, 5.0f);
        ImGui::InputFloat("Outer Angle", &outer_angle, 1.0f, 5.0f);


        lc->SetInnerAngle(inner_angle);
        lc->SetOuterAngle(outer_angle);

      }

      bool before = lc->CanCastShadows();

      if (lc->GetType() != LightComponent::kPoint)
      {
        if (ImGui::Checkbox("Can cast shadows", &before))
        {
          lc->SetCanCastShadows(before);
          render.CallClearFrame();
        }
      }
    }


    // Transformations with guizmo based on current mode
    if (trs != nullptr)
    {
      ImGui::Text("Dynamic");

      if (ImGui::RadioButton("Translate", eu->current_guizmo_opeation_ == ImGuizmo::TRANSLATE))
      {
        eu->current_guizmo_opeation_ = ImGuizmo::TRANSLATE;
      }

      ImGui::SameLine();

      if (ImGui::RadioButton("Rotate", eu->current_guizmo_opeation_ == ImGuizmo::ROTATE))
      {
        eu->current_guizmo_opeation_ = ImGuizmo::ROTATE;
      }

      ImGui::SameLine();

      if (ImGui::RadioButton("Scale", eu->current_guizmo_opeation_ == ImGuizmo::SCALE))
      {
        eu->current_guizmo_opeation_ = ImGuizmo::SCALE;
        eu->current_guizmo_mode_ = ImGuizmo::LOCAL;
      }

      if (eu->current_guizmo_opeation_ != ImGuizmo::SCALE)
      {
        if (ImGui::RadioButton("Local", eu->current_guizmo_mode_ == ImGuizmo::LOCAL))
          eu->current_guizmo_mode_ = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", eu->current_guizmo_mode_ == ImGuizmo::WORLD))
          eu->current_guizmo_mode_ = ImGuizmo::WORLD;
      }
      ImGui::Spacing();


      float position[3] = { trs->position_.x, trs->position_.y, trs->position_.z };
      float rotation[3] = { trs->rotation_.x, trs->rotation_.y, trs->rotation_.z };
      float scale[3] = { trs->scale_.x, trs->scale_.y, trs->scale_.z };

      PrintTRS(position, rotation, scale, true);

      trs->SetPosition(position[0], position[1], position[2]);
      trs->SetRotation(rotation[0], rotation[1], rotation[2]);
      trs->SetScale(scale[0], scale[1], scale[2]);
    }
    else if (lt != nullptr)
    {
      ImGui::Text("Static");

      float position[3] = { lt->position_.x, lt->position_.y, lt->position_.z };
      float rotation[3] = { lt->rotation_.x, lt->rotation_.y, lt->rotation_.z };
      float scale[3] = { lt->scale_.x, lt->scale_.y, lt->scale_.z };
      PrintTRS(position, rotation, scale, false);
    }
    else if (wt != nullptr)
    {
      ImGui::Text("Static");
      float position[3] = { wt->position_.x, wt->position_.y, wt->position_.z };
      float rotation[3] = { wt->rotation_.x, wt->rotation_.y, wt->rotation_.z };
      float scale[3] = { wt->scale_.x, wt->scale_.y, wt->scale_.z };
      PrintTRS(position, rotation, scale, false);
    }

    ImGui::Spacing();
    //Parent

    if (lt != nullptr)
    {


      int pos = 0;
      uint32 id = 0;
      if (lt->GetParent() != nullptr)
      {
        id = ecs.RealEntityId(lt->GetParent()->id_);
      }

      if(id!=0)
      {
        for (int i = 0; i < eu->entities_ptr_.size(); ++i)
        {
          if(eu->entities_ptr_.at(i)!= nullptr)
          {
            if(ecs.RealEntityId(eu->entities_ptr_.at(i)->id_) == id)
            {
              pos = i;
            }
          }

        }

      }

      if (ImGui::Combo("Parent", &pos, vector_getter, static_cast<void*>(&eu->id_vector_), eu->id_vector_.size())) // Set entity parent
      {

        if (pos != 0)
        {
          Entity* s_parent = eu->entities_ptr_.at(pos);
          if (s_parent != ent)
          {
            s_parent->SetChildren(ent);
            //lt->SetParent(s_parent);
          }
        }
        else
        {
          lt->SetParent(nullptr);
        }
      }
    }

    ImGui::Spacing();

    if (rc)
    {

      ImGui::Checkbox("Can throw shadows", &rc->b_throw_shadows); // toggle shadows

      std::vector<std::string> geometries;
      std::vector<int> id_geometries;

      geometries.push_back("None");
      id_geometries.push_back(-1);

      int pos_geo = 0;

      for (auto it = render.geometry_map_.begin(); it != render.geometry_map_.end(); ++it)
      {
        geometries.push_back(std::to_string(it->second->id_));
        id_geometries.push_back(it->second->id_);
        if (rc->GetMesh() != nullptr)
        {
          if (rc->GetMesh()->id_ == it->second->id_)
          {
            pos_geo = geometries.size() - 1;
          }
        }
      }

      if (ImGui::Combo("Geometry", &pos_geo, vector_getter, static_cast<void*>(&geometries), geometries.size()))
      {
        if (pos_geo != 0)
        {
          rc->SetMesh(id_geometries.at(pos_geo));
        }
        else
        {
          rc->QuitMesh();
        }
      }
    }


    // Add components
    if (wt == nullptr)
    {
      if (ImGui::Button("Add World Component"))
      {
        ecs.NewWorldTransform(ent);
      }
    }
    if (lt == nullptr)
    {
      if (ImGui::Button("Add Local Component"))
      {
        if (wt != nullptr)
        {
          ecs.NewLocalTransform(ent)->SetLocal(wt->position_, wt->rotation_, wt->scale_);
        }
        else
        {
          ecs.NewLocalTransform(ent)->SetLocal(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        }

      }
    }
    if (trs == nullptr)
    {
      if (ImGui::Button("Add Local TRS Component")) // Make entity dynamic
      {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        if (wt == nullptr)
        {
          ecs.NewWorldTransform(ent);
        }
        else
        {
          position = wt->position_;
          rotation = wt->rotation_;
          scale = wt->scale_;
        }
        if (lt == nullptr)
        {
          ecs.NewLocalTransform(ent);
        }
        else
        {
          position = lt->position_;
          rotation = lt->rotation_;
          scale = lt->scale_;
        }

        LocalTransformTRS *trs_e = ecs.NewLocalTransformTRS(ent);
        trs_e->SetPosition(position);
        trs_e->SetRotation(rotation);
        trs_e->SetScale(scale);
      }
    }

    if (rc == nullptr)
    {
      if (ImGui::Button("Add New Render Component")) // Add mesh to an entity
      {
        ecs.NewRenderComponent(ent);
      }
    }
  }


  ImGui::End();

  //Image video
  ImGui::Begin("World", nullptr, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

  ImVec2 s_size = ImVec2(EngineUtils::Instance().width, EngineUtils::Instance().height);
  ImVec2 result = ImVec2((ImGui::GetWindowSize().x - s_size.x)*0.5f,
    (ImGui::GetWindowSize().y - s_size.y)*0.5f);
  ImGui::SetCursorPos(result);

  // Display texture on the wolrd window based on the render state.
  if (EngineUtils::Instance().active_deferred_)
  {

    ImGui::Image((ImTextureID)RenderList::Instance().deferred_fb_.g_normal_texture_,
      s_size, ImVec2(0, 1), ImVec2(1, 0));
  }
  else
  {
    if (RenderList::Instance().camera_->bloom_active_)
    {
      ImGui::Image((ImTextureID)RenderList::Instance().bloom_fb_.bloom_buffers_[0],
        s_size, ImVec2(0, 1), ImVec2(1, 0));
    }
    else
    {
      ImGui::Image((ImTextureID)RenderList::Instance().principal_fb_.texture_id_,
        s_size, ImVec2(0, 1), ImVec2(1, 0));
    }
  }

  // ImGuizmo transformations

  float* local_matrix = (float*)glm::value_ptr(lt->local_);
  float* world_matrix = (float*)glm::value_ptr(wt->world_);


  if (trs != nullptr && !eu->play_update_)
  {
    // If entity has parent we need to extract the local position from the parent with the inverse of its world matrix
    if (lt->GetParent() != nullptr)
    {
      ImGuizmo::SetDrawlist();
      ImVec2 pos_rect;
      pos_rect.x = ImGui::GetWindowPos().x - ((s_size.x* 0.5f) - (ImGui::GetWindowSize().x * 0.5f));
      pos_rect.y = ImGui::GetWindowPos().y - ((s_size.y* 0.5f) - (ImGui::GetWindowSize().y * 0.5f));
      ImGuizmo::SetRect(pos_rect.x, pos_rect.y, s_size.x, s_size.y);

      ImGuizmo::RecomposeMatrixFromComponents(&wt->position_.x, &wt->rotation_.x, &wt->scale_.x, world_matrix);
      if (ImGuizmo::Manipulate((const float*)glm::value_ptr(render.camera_->view_),
        (const float*)glm::value_ptr(render.camera_->projection_),
        eu->current_guizmo_opeation_, eu->current_guizmo_mode_,
        world_matrix))
      {

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(world_matrix, matrixTranslation, matrixRotation, matrixScale);

        glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]));

        glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(matrixRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(matrixRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(matrixRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 translationMatrix = glm::translate((glm::mat4(1.0f), glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2])));

        glm::mat4 model = translationMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrixX * scalingMatrix;

        WorldTransform* tmp = ECS::Instance().GetWorldTransform(lt->GetParent());

        glm::mat4 result = glm::inverse(tmp->world_) * model;
        float* final_model = (float*)glm::value_ptr(result);

        ImGuizmo::DecomposeMatrixToComponents(final_model, matrixTranslation, matrixRotation, matrixScale);

        switch (eu->current_guizmo_opeation_)
        {
        case ImGuizmo::ROTATE:
          trs->SetRotation(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
          break;
        case ImGuizmo::TRANSLATE:
          trs->SetPosition(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
          break;
        case ImGuizmo::SCALE:
          trs->SetScale(matrixScale[0], matrixScale[1], matrixScale[2]);
          break;
        default:;
        }
      }
    }
    else  // If entity has no transform normally
    {
      ImGuizmo::SetDrawlist();
      ImVec2 pos_rect;
      pos_rect.x = ImGui::GetWindowPos().x - ((s_size.x* 0.5f) - (ImGui::GetWindowSize().x * 0.5f));
      pos_rect.y = ImGui::GetWindowPos().y - ((s_size.y* 0.5f) - (ImGui::GetWindowSize().y * 0.5f));
      ImGuizmo::SetRect(pos_rect.x, pos_rect.y, s_size.x, s_size.y);

      ImGuizmo::RecomposeMatrixFromComponents(&lt->position_.x, &lt->rotation_.x, &lt->scale_.x, local_matrix);
      if (ImGuizmo::Manipulate((const float*)glm::value_ptr(render.camera_->view_),
        (const float*)glm::value_ptr(render.camera_->projection_),
        eu->current_guizmo_opeation_, eu->current_guizmo_mode_,
        local_matrix))
      {

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(local_matrix, matrixTranslation, matrixRotation, matrixScale);

        switch (eu->current_guizmo_opeation_)
        {
        case ImGuizmo::ROTATE:
          trs->SetRotation(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
          break;
        case ImGuizmo::TRANSLATE:
          trs->SetPosition(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
          break;
        case ImGuizmo::SCALE:
          trs->SetScale(matrixScale[0], matrixScale[1], matrixScale[2]);
          break;
        default:;
        }
      }
    }
  }

  eu->world_window_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
  eu->world_window_pos = glm::vec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y);

  ImGui::End();

  //Debug Shadow textures
  for (int i = 0; i < RenderList::Instance().kMaxShadows; ++i)
  {
    ImGui::Begin(std::string(std::string("Shadow") + std::to_string(i)).c_str());
    ImGui::Image((ImTextureID)RenderList::Instance().shadow_fb_[i].texture_id_, ImGui::GetWindowSize()
      , ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

  }

  ImGui::Render();
  RenderList::Instance().coal_imgui->ImguiDraw();

#ifdef JSON_MEASURE
  MTR_END("third", "Imgui operations");
#endif

}
