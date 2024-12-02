#define JSON_MEASURE 1
#include <entry.h>
#include <px_mem.h>
#include <px_sched.h>

#include "coalenginelua.h"
#include "displaylist.h"
#include "ecs.h"
#include "engineutils.h"
#include "renderlist.h"
#include "minitrace.h"
#include "geometrybuilder.h"
#include "materialbuilder.h"
#include "texturebuilder.h"
#include "timer.h"
#include "framebuffer.h"
#include "coalimgui.h"





coalengine::Camera cam_;

void coalengine::Start()
{




  /*Default camera init*/
  cam_.SetPosition(glm::vec3(0.0f, 2.0f, 5.0f));
  cam_.SetFront(glm::vec3(0.0f, 0.0f, -1.0f));
  cam_.SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
  cam_.SetSpeed(10.0f);
  cam_.SetYaw(-90.0f);
  cam_.SetPitch(0.0f);
  cam_.SetLastX(EngineUtils::Instance().width / 2.0f);
  cam_.SetLastY(EngineUtils::Instance().height / 2.0f);

  /*Default skybox init*/
  std::vector<std::string> faces =
  {
    "../../../data/Textures/SkyBox/posx.jpg",
    "../../../data/Textures/SkyBox/negx.jpg",
    "../../../data/Textures/SkyBox/posy.jpg",
    "../../../data/Textures/SkyBox/negy.jpg",
    "../../../data/Textures/SkyBox/posz.jpg",
    "../../../data/Textures/SkyBox/negz.jpg",
  };

  cam_.SetSkyBox(faces);
  RenderList::Instance().SetCamera(&cam_);

  /*Post-process examples*/
  //cam_.AddPostprocess(PostProcess::kGrayScale);
  //cam_.AddPostprocess(PostProcess::kInversion);
  //cam_.AddPostprocess(PostProcess::kKernelEffect);

  /*Example for read lua on start*/
  //CoalEngineLua::Instance().RunScript("file/to/read");
  CoalEngineLua::Instance().RunScript("../../../data/Scripts/anibalscene.lua");


}

void coalengine::Update()
{
#ifdef JSON_MEASURE
  MTR_SCOPE("thread", "Update");
#endif
  /*Example for read lua on update*/
  //CoalEngineLua::Instance().RunScript("file/to/read");
  CoalEngineLua::Instance().RunScript("../../../data/Scripts/anibalupdate.lua");


}



