#include "renderlist.h"

#include <px_sched.h>
#include <stdio.h>
#include <ImGuizmo.h>
#include "ecs.h"
#include "engineutils.h"
#include "minitrace.h"
#include "texture.h"



coalengine::RenderList& coalengine::RenderList::Instance()
{
  static RenderList* inst = nullptr;
  if (inst == nullptr) {
    inst = new RenderList();
  }
  return *inst;
}


coalengine::RenderList::RenderList()
{
  coal_imgui.alloc();
  state_ = kNone;
  num_dp_to_exec_ = 0;
  num_geometries_ = 0;
  num_textures_ = 0;
  num_materials_ = 0;
  material_default_ = 0;
  material_shadow_ = 0;
  material_deferred_ = 0;
  lights_.light_w_shadows_ = 0;
  actual_s_light_ = 0;
  clear_frame_ = false;
  camera_ = nullptr;
}


coalengine::RenderList::~RenderList()
{

}


void coalengine::RenderList::Execute()
{
  //mtx_dp.lock();

  int size = draw_render_list_.size();

 
  for (int i = 0; i < size; ++i)
  {
    draw_render_list_.front()->RunAll();
    draw_render_list_.pop_front();
  }
  //mtx_dp.unlock();
}


void coalengine::RenderList::Swap()
{
  draw_render_list_ = std::move(render_list_);
}


void coalengine::RenderList::SubmitDisplayList(px::Mem<DisplayList>&& dp)
{
  mtx_dp.lock();
  render_list_.push_back(std::move(dp));
  mtx_dp.unlock();
}

void coalengine::RenderList::SubmitGeometry(px::Mem<Geometry>&& geometry)
{
  mtx_geometry.lock();
  geometry_map_.insert(std::make_pair(geometry->id_, std::move(geometry)));
  mtx_geometry.unlock();
}

void coalengine::RenderList::SubmitMaterial(px::Mem<Material>&& material)
{
  mtx_material.lock();
  material_map_.insert(std::make_pair(material->id_, std::move(material)));
  mtx_material.unlock();
}

void coalengine::RenderList::SubmitTexture(px::Mem<Texture>&& texture)
{
  mtx_texture.lock();
  textures_to_upload_.emplace_back(texture->id_);
  texture_map_.insert(std::make_pair(texture->id_, std::move(texture)));

  mtx_texture.unlock();
}

bool coalengine::RenderList::IsGeometry(uint32 num)
{
  int res = num & 0xF0000000;
  if (res == 0x10000000)
  {
    return true;
  }
  return false;
}

bool coalengine::RenderList::IsTexture(uint32 num)
{
  int res = num & 0xF0000000;
  if (res == 0x20000000)
  {
    return true;
  }
  return false;
}

bool coalengine::RenderList::IsMaterial(uint32 num)
{
  int res = num & 0xF0000000;
  if (res == 0x30000000)
  {
    return true;
  }
  return false;
}

bool coalengine::RenderList::IsValidGeometry(uint32 id)
{
  if (IsGeometry(id))
  {
    std::map<uint32, px::Mem<Geometry>>::iterator it =
      geometry_map_.find(id);

    if (it != geometry_map_.end())
    {
      return true;
    }
  }
  return false;
}

// Thread to load heavy textures
void coalengine::RenderList::StartLoadTextures()
{
  if (textures_to_upload_.size() > 0)
  {
    px_sched::Scheduler schd;
    schd.init();
    px_sched::Sync s;
    mtx_texture.lock();
    for (unsigned int i = 0; i < textures_to_upload_.size(); i++)
    {
      uint32 text_id = textures_to_upload_.at(i);
      auto job = [text_id] {

        Instance().texture_map_[text_id].get()->LoadTexture();

      };
      schd.run(job, &s);
    }
    schd.waitFor(s);

    textures_to_upload_.clear();
    mtx_texture.unlock();
  }
}

void coalengine::RenderList::CallClearFrame()
{
  clear_frame_ = true;
}

void coalengine::RenderList::ClearFrame()
{

  if(clear_frame_)
  {
    draw_render_list_.clear();
    clear_frame_ = false;
  }
}

void coalengine::RenderList::SetCamera(Camera* camera)
{
  camera_ = camera;
}

void coalengine::RenderList::ClearLights()
{
  actual_s_light_ = 0;
  // NO Shadow
  lights_.lightdirpos_.clear();
  lights_.lightdirdir_.clear();
  lights_.lightdircolor_.clear();
  lights_.lightdirintensity_.clear();
  lights_.lightpointpos_.clear();
  lights_.lightpointdir_.clear();
  lights_.lightpointcolor_.clear();
  lights_.lightpointintensity_.clear();
  lights_.lightspotpos_.clear();
  lights_.lightspotdir_.clear();
  lights_.lightspotcolor_.clear();
  lights_.lightspotinnerangle_.clear();
  lights_.lightspotouterangle_.clear();
  lights_.lightspotintensity_.clear();

  // WITH Shadow

  lights_.lightdirposshadow_.clear();
  lights_.lightdirdirshadow_.clear();
  lights_.lightdircolorshadow_.clear();
  lights_.light_spaces_dir_.clear();
  lights_.shadow_map_dir_.clear();
  lights_.lightpointposshadow_.clear();
  lights_.lightpointdirshadow_.clear();
  lights_.lightpointcolorshadow_.clear();
  lights_.lightspotposshadow_.clear();
  lights_.lightspotdirshadow_.clear();
  lights_.lightspotcolorshadow_.clear();
  lights_.lightspotinnerangleshadow_.clear();
  lights_.lightspotouterangleshadow_.clear();
  lights_.light_spaces_spot_.clear();
  lights_.shadow_map_spot_.clear();
  lights_.light_spaces_.clear();
  lights_.light_w_shadows_ = 0;
  lights_.lightdirintensityshadow_.clear();
  lights_.lightspotintensityshadow_.clear();
}

void coalengine::RenderList::mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{


  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT == GLFW_PRESS))
  {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // if is the first mouse click ste the last position to the current mouse position
    if (RenderList::Instance().camera_->first_mouse_)
    {
      RenderList::Instance().camera_->SetLastX(xpos);
      RenderList::Instance().camera_->SetLastY(ypos);
      RenderList::Instance().camera_->first_mouse_ = false;
    }

    // Get movement offset respect from last frame
    double xoffset = xpos - RenderList::Instance().camera_->GetLastX();
    double yoffset = RenderList::Instance().camera_->GetLastY() - ypos;

    //Set the new last position
    RenderList::Instance().camera_->SetLastX(xpos);
    RenderList::Instance().camera_->SetLastY(ypos);

    // Set ssensitivity
    double sensitivity = 0.1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    RenderList::Instance().camera_->SetPitch(RenderList::Instance().camera_->GetPitch() + yoffset);
    RenderList::Instance().camera_->SetYaw(RenderList::Instance().camera_->GetYaw() + xoffset);

    if (RenderList::Instance().camera_->GetPitch() > 89.0f)
      RenderList::Instance().camera_->SetPitch(89.0f);
    if (RenderList::Instance().camera_->GetPitch() < -89.0f)
      RenderList::Instance().camera_->SetPitch(-89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(RenderList::Instance().camera_->GetYaw())) * cos(glm::radians(RenderList::Instance().camera_->GetPitch()));
    direction.y = sin(glm::radians(RenderList::Instance().camera_->GetPitch()));
    direction.z = sin(glm::radians(RenderList::Instance().camera_->GetYaw())) * cos(glm::radians(RenderList::Instance().camera_->GetPitch()));
    RenderList::Instance().camera_->SetFront(glm::normalize(direction));
  }
  else
  {
    // button release
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    RenderList::Instance().camera_->first_mouse_ = true;
  }
}


void coalengine::RenderList::mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
  // not raycasting if user its using guizmo to prevent unwanted behavior


  if (ImGuizmo::IsOver())
  {
    return;
  }



  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {

    float minDistance = 1000000.0f;
    Entity* closestEntity = nullptr;
    // Get mouse position in viewport space
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Mouse pos out of bounds world window
    if (xpos < EngineUtils::Instance().world_window_pos.x
      || xpos > EngineUtils::Instance().world_window_pos.x + EngineUtils::Instance().world_window_size.x
      || ypos < EngineUtils::Instance().world_window_pos.y
      || ypos > EngineUtils::Instance().world_window_pos.y + EngineUtils::Instance().world_window_size.y)
    {
      return;
    }

    // Convert mouse position from glm window to imgui window
    float tmp_pos_x = EngineUtils::Instance().world_window_pos.x;
    float tmp_pos_y = EngineUtils::Instance().world_window_pos.y;

    float tmp_size_x = EngineUtils::Instance().world_window_size.x;
    float tmp_size_y = EngineUtils::Instance().world_window_size.y;

    float center_x = EngineUtils::Instance().world_window_pos.x + EngineUtils::Instance().world_window_size.x / 2.0f;
    float center_y = EngineUtils::Instance().world_window_pos.y + EngineUtils::Instance().world_window_size.y / 2.0f;

    xpos += EngineUtils::Instance().width / 2.0f - center_x;
    ypos += EngineUtils::Instance().height / 2.0f - center_y;

    // Normalized Device Coordinates
    float x = (2.0f * xpos) / 1920.0f - 1.0f;
    float y = 1.0f - (2.0f * ypos) / 1080.0f;
    float z = 1.0f;
    glm::vec3 ray_nds = glm::vec3(x, y, z);

    // Homogeneous Clip Coordinates
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

    //Camera coordinates
    glm::vec4 ray_eye = glm::inverse(RenderList::Instance().camera_->projection_) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);


    // World Coordinates
    glm::vec3 ray_wor = glm::inverse(RenderList::Instance().camera_->view_) * ray_eye;
    ray_wor = normalize(ray_wor);

    ECS& ecs = ECS::Instance();

    // Loop through all entities that has render and check if the ray cast has intersected with its AABB
    for (std::map<uint32, px::Mem<Entity>>::iterator it = ecs.entities_map_.begin(); it != ecs.entities_map_.end(); it++)
    {
      if (it->second->HasComponents(Entity::RENDER))
      {
        float Distance = 0.0f;
        RenderComponent* render_comp = ecs.GetRenderComponent(it->second.get());
        WorldTransform* world_comp = ecs.GetWorldTransform(it->second.get());

        BoundingBox box = render_comp->box;

        box.min_x = box.max_x = 0.0f;
        box.min_y = box.max_y = 0.0f;
        box.min_z = box.max_z = 0.0f;


        // Rotate bounding box based on mesh rotation
        for (unsigned int i = 0; i < box.corners.size(); ++i)
        {

          glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(world_comp->rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
          glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(world_comp->rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
          glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(world_comp->rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));


          box.corners.at(i) = rotationMatrixX * rotationMatrixY * rotationMatrixZ * box.corners.at(i);

          if (box.corners.at(i).x < box.min_x) box.min_x = box.corners.at(i).x;
          if (box.corners.at(i).x > box.max_x) box.max_x = box.corners.at(i).x;
          if (box.corners.at(i).y < box.min_y) box.min_y = box.corners.at(i).y;
          if (box.corners.at(i).y > box.max_y) box.max_y = box.corners.at(i).y;
          if (box.corners.at(i).z < box.min_z) box.min_z = box.corners.at(i).z;
          if (box.corners.at(i).z > box.max_z) box.max_z = box.corners.at(i).z;

        }

        // Scale AABB based on mesh scale
        box.min_x *= world_comp->scale_.x;
        box.max_x *= world_comp->scale_.x;
        box.min_y *= world_comp->scale_.y;
        box.max_y *= world_comp->scale_.y;
        box.min_z *= world_comp->scale_.z;
        box.max_z *= world_comp->scale_.z;

        //Translate AABB based on mesh position
        box.min_x += world_comp->position_.x;
        box.max_x += world_comp->position_.x;
        box.min_y += world_comp->position_.y;
        box.max_y += world_comp->position_.y;
        box.min_z += world_comp->position_.z;
        box.max_z += world_comp->position_.z;

        if (RaycastAABBIntersection(ray_wor, box, Distance))
        {
          // Find the closest intersection
          if (minDistance > Distance)
          {
            minDistance = Distance;
            closestEntity = it->second.get();
          }
        }
      }
    }
    // Set the entity intersected
    if (closestEntity != nullptr)
    {
      EngineUtils::Instance().entity_id_sel_ = ecs.RealEntityId(closestEntity->id_);
    }
  }
}

void  coalengine::RenderList::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // Full screen
  if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
  {
    coalengine::EngineUtils::Instance().AddToLog("Show Front End");
    coalengine::EngineUtils::Instance().show_engine_front_ = !coalengine::EngineUtils::Instance().show_engine_front_;
  }
}

bool coalengine::RenderList::RaycastAABBIntersection(glm::vec3 ray, BoundingBox box, float& distance)
{
  // Min/Max AABB intersection
  float t1 = (box.min_x - Instance().camera_->position_.x) / ray.x;
  float t2 = (box.max_x - Instance().camera_->position_.x) / ray.x;
  float t3 = (box.min_y - Instance().camera_->position_.y) / ray.y;
  float t4 = (box.max_y - Instance().camera_->position_.y) / ray.y;
  float t5 = (box.min_z - Instance().camera_->position_.z) / ray.z;
  float t6 = (box.max_z - Instance().camera_->position_.z) / ray.z;

  float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
  float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

  // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
  if (tmax < 0.0f) {
    distance = tmax;
    return false;
  }

  // if tmin > tmax, ray doesn't intersect AABB
  if (tmin > tmax) {
    distance = tmax;
    return false;
  }

  distance = tmin;
  return true;
}
