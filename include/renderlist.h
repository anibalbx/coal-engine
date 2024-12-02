#ifndef __RENDERLIST_H__
#define __RENDERLIST_H__ 1
#include <fwd.hpp>
#include <list>
#include <vec3.hpp>
#include <vector>
#include <map>
#include <mutex>

#include "cubemap.h"
#include "displaylist.h"
#include "camera.h"
#include "framebufferbloom.h"
#include "framebufferdeferred.h"
#include "framebuffershadow.h"
#include "coalimgui.h"
#include "boundingbox.h"
#include "postprocess.h"

namespace coalengine
{
  class RenderList
  {
  public:

    static RenderList& Instance();


    enum RenderState
    {
      kNone,
      kShadowRenderST,
      kNormalRenderST,
      kHDRRenderST,
      kBloomRenderST,
      kDeferredRenderST,
    };

    struct Lights
    {
      // NO Shadow
      // Maps to store all directional lights info.
      std::vector<glm::vec3> lightdirpos_;
      std::vector<glm::vec3> lightdirdir_;
      std::vector<glm::vec3> lightdircolor_;
      std::vector<float> lightdirintensity_;

      // Maps to store all point lights info.
      std::vector<glm::vec3> lightpointpos_;
      std::vector<glm::vec3> lightpointdir_;
      std::vector<glm::vec3> lightpointcolor_;
      std::vector<float> lightpointintensity_;

      // Maps to store all spot lights info.
      std::vector<glm::vec3> lightspotpos_;
      std::vector<glm::vec3> lightspotdir_;
      std::vector<glm::vec3> lightspotcolor_;
      std::vector<float> lightspotinnerangle_;
      std::vector<float> lightspotouterangle_;
      std::vector<float> lightspotintensity_;
    
      // WITH Shadow
       // Maps to store all directional lights info.
      std::vector<glm::vec3> lightdirposshadow_;
      std::vector<glm::vec3> lightdirdirshadow_;
      std::vector<glm::vec3> lightdircolorshadow_;
      std::vector<float> lightdirintensityshadow_;
      std::vector<glm::mat4> light_spaces_dir_;
      std::vector<int> shadow_map_dir_;

      // Maps to store all point lights info.
      std::vector<glm::vec3> lightpointposshadow_;
      std::vector<glm::vec3> lightpointdirshadow_;
      std::vector<glm::vec3> lightpointcolorshadow_;
      std::vector<float> lightpointintensityshadow_;

      // Maps to store all spot lights info.
      std::vector<glm::vec3> lightspotposshadow_;
      std::vector<glm::vec3> lightspotdirshadow_;
      std::vector<glm::vec3> lightspotcolorshadow_;
      std::vector<float> lightspotinnerangleshadow_;
      std::vector<float> lightspotouterangleshadow_;
      std::vector<float> lightspotintensityshadow_;
      std::vector<glm::mat4> light_spaces_spot_;
      std::vector<int> shadow_map_spot_;


      // Lights space matrix to shadow mapping
      std::vector<glm::mat4> light_spaces_;

      // number light with shadows.
      uint8 light_w_shadows_;
    };


    RenderState state_;

    // Display list stored.
    std::list<px::Mem<DisplayList>> render_list_;
    std::list<px::Mem<DisplayList>> draw_render_list_;

    std::map<uint32_t, px::Mem<Geometry>> geometry_map_;
    std::map<uint32_t, px::Mem<Material>> material_map_;
    std::map<uint32_t, px::Mem<Texture>> texture_map_;
    std::vector <uint32_t> textures_to_upload_;


    CubeMap sky_box_;

    uint32 num_geometries_;
    uint32 num_textures_;
    uint32 num_materials_;

    // Num display list to execute.
    uint8 num_dp_to_exec_;

    // Different material depending on the state of render.
    uint32 material_default_;
    uint32 material_shadow_;
    uint32 material_deferred_;

    std::mutex mtx_dp;
    std::mutex mtx_geometry;
    std::mutex mtx_texture;
    std::mutex mtx_material;

    static const uint8 kMaxShadows = 4;
    // Main camera.

    Camera* camera_;
    // TODO multiple post process
    std::vector<px::Mem<PostProcess>> post_process_list_;
    //PostProcess post_process_;

    Lights lights_;
    uint8 actual_s_light_;
    // Different framebuffers baes on the render state-
    FrameBuffer principal_fb_;
    FrameBufferBloom bloom_fb_;
    FrameBufferG deferred_fb_;
    FrameBufferShadow shadow_fb_[kMaxShadows];

    px::Mem<CoalImgui> coal_imgui;
    bool clear_frame_;
  

    RenderList();

    RenderList(const RenderList& o) = default;


    ~RenderList();

    void Execute();

    void Swap();

    void SubmitDisplayList(px::Mem<DisplayList>&& dp);

    void SubmitGeometry(px::Mem<Geometry>&& geometry);

    void SubmitMaterial(px::Mem<Material>&& material);

    void SubmitTexture(px::Mem<Texture>&& texture);

    // checks if the given id is a geometry.
    bool IsGeometry(uint32_t num);

    // checks if the given id is a texture.
    bool IsTexture(uint32_t num);

    // checks if the given id is a material.
    bool IsMaterial(uint32_t num);

    bool IsValidGeometry(uint32_t id);

    void StartLoadTextures();

    void CallClearFrame();
    void ClearFrame();

    void SetCamera(Camera* camera);

    void ClearLights();

    // Pos callback to look around with the mouse.
    static void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos);

    // click callback to look select entities.
    static void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);

    // Input callback
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // RAy cast intersection between ray from viewer position from world position and axis aliased bounding box.
    static bool RaycastAABBIntersection(glm::vec3 ray, BoundingBox box, float& distance);

  };
}



#endif