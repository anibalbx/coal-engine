#define PX_SCHED_IMPLEMENTATION 1
#define PX_MEM_IMPLEMENTATION 1
#define JSON_MEASURE 1
//#define SINGLE_CORE_ECS 1

#include <entry.h>
#include <px_sched.h>
#include "glm/glm.hpp"
#include "coalimgui.h"
#include "minitrace.h"
#include <glm/gtc/matrix_transform.hpp>
#include "renderlist.h"
#include "ecs.h"
#include "engineutils.h"

/*Init lambdas for jobs*/

auto job_create_delete = []
{
#ifdef JSON_MEASURE
  MTR_SCOPE("EU", "Create-Delete");
#endif
  coalengine::ECS::Instance().CreateEngineInstances();
  coalengine::ECS::Instance().DeleteEntitiesSystem();
};

auto job_update_trs = []
{
#ifdef JSON_MEASURE
  MTR_SCOPE("EU", "TRS");
#endif
  coalengine::ECS::Instance().UpdateTRSSystem();
  coalengine::ECS::Instance().UpdateWorldLocalWithTRSSystem();
};

auto job_update_semi = []
{
#ifdef JSON_MEASURE
  MTR_SCOPE("EU", "Semi-Static");
#endif
  coalengine::ECS::Instance().UpdateWorldLocalNoTRSSystem();
};
auto job_lights = []
{
#ifdef JSON_MEASURE
  MTR_SCOPE("EU", "Lights");
#endif
  coalengine::ECS::Instance().UpdateLights();
};



void EngineUpdateSingleThread()
{

#ifdef JSON_MEASURE
  MTR_SCOPE("second", "Engine Update");
#endif


  coalengine::ECS::Instance().CreateEngineInstances();
  coalengine::ECS::Instance().DeleteEntitiesSystem();
  coalengine::ECS::Instance().UpdateTRSSystem();
  coalengine::ECS::Instance().UpdateWorldLocalWithTRSSystem();
  coalengine::ECS::Instance().UpdateWorldLocalNoTRSSystem();
  coalengine::ECS::Instance().UpdateLights();
  coalengine::ECS::Instance().SepareLights();
  coalengine::ECS::Instance().RenderSystem();

}

void ProcessDisplayList()
{

#ifdef JSON_MEASURE
  MTR_SCOPE("thread", "Process");
#endif
  coalengine::RenderList& RL = coalengine::RenderList::Instance();

  coalengine::EngineUtils::Instance().active_deferred_ = false;

 

  for (unsigned int i = 0; i < RL.lights_.light_w_shadows_; ++i)
  {
    px::Mem <coalengine::DisplayList> dpshadows;
    dpshadows.alloc();
    dpshadows->SetFramebuffer(&RL.shadow_fb_[i]);
    dpshadows->clearScreen();
    dpshadows->DrawAllEntities();
    RL.SubmitDisplayList(std::move(dpshadows));
  }


  if (!coalengine::EngineUtils::Instance().active_deferred_)
  {
      px::Mem <coalengine::DisplayList> dpupdate;
      dpupdate.alloc();
      if (RL.camera_->bloom_active_)
      {
          dpupdate->SetFramebuffer(&RL.bloom_fb_);

      }
      else
      {
          dpupdate->SetFramebuffer(&RL.principal_fb_);
      }

      dpupdate->clearScreen();
      dpupdate->DrawAllEntities();
      dpupdate->DrawSkyBox();
      dpupdate->ApplyPostProcess();
      dpupdate->ClearEntities();
      RL.SubmitDisplayList(std::move(dpupdate));
  }else
  {
      px::Mem <coalengine::DisplayList> dpupdate;
      dpupdate.alloc();          
      dpupdate->SetFramebuffer(&RL.deferred_fb_);
      dpupdate->clearScreen();
      dpupdate->DrawAllEntities();
      dpupdate->DrawSkyBox();
      dpupdate->ClearEntities();
      RL.SubmitDisplayList(std::move(dpupdate));
  }
 

  
	
  if(coalengine::EngineUtils::Instance().show_engine_front_)
  {
    px::Mem <coalengine::DisplayList> dpimgui;
    dpimgui.alloc();
    dpimgui->SetFramebuffer(nullptr);
    dpimgui->DpImgui();
    RL.SubmitDisplayList(std::move(dpimgui));
  }

}


void processInput(GLFWwindow* window, float& deltaTime, float& lastFrame)
{

  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

    const float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      coalengine::RenderList::Instance().camera_->SetPosition(coalengine::RenderList::Instance().camera_->GetPosition() + 
                                                                  coalengine::RenderList::Instance().camera_->GetSpeed() * deltaTime * 
                                                                 coalengine::RenderList::Instance().camera_->GetFront());
                                                            
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    coalengine::RenderList::Instance().camera_->SetPosition(coalengine::RenderList::Instance().camera_->GetPosition() -
                                                               coalengine::RenderList::Instance().camera_->GetSpeed() * deltaTime *
                                                               coalengine::RenderList::Instance().camera_->GetFront());
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    coalengine::RenderList::Instance().camera_->SetPosition(coalengine::RenderList::Instance().camera_->GetPosition() - glm::normalize(glm::cross(coalengine::RenderList::Instance().camera_->GetFront(),
                                                              coalengine::RenderList::Instance().camera_->GetUp())) *
                                                               coalengine::RenderList::Instance().camera_->GetSpeed() * deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    coalengine::RenderList::Instance().camera_->SetPosition(coalengine::RenderList::Instance().camera_->GetPosition() + glm::normalize(glm::cross(coalengine::RenderList::Instance().camera_->GetFront(),
                                                             coalengine::RenderList::Instance().camera_->GetUp())) *
                                                               coalengine::RenderList::Instance().camera_->GetSpeed() * deltaTime);

}

void render()
{
#ifdef JSON_MEASURE
  MTR_SCOPE("first", "render");
#endif
  coalengine::RenderList& RL = coalengine::RenderList::Instance();
  RL.Execute();

}

int main(int argc, char** argv)
{
#ifdef JSON_MEASURE
  mtr_init("trace.json");
  MTR_META_PROCESS_NAME("coal_engine_trace");
  MTR_META_THREAD_NAME("main thread");
  int time_trace;
  MTR_START("background", "main", &time_trace);
#endif

  coalengine::EngineUtils::Instance().width = 1920;
  coalengine::EngineUtils::Instance().height = 1080;

  float deltaTime = 0.0f;	// Time between current frame and last frame
  float lastFrame = 0.0f; // Time of last frame



/*Window init*/
  px::Mem <Window> wind;
  wind.alloc();
  wind.get()->init(coalengine::EngineUtils::Instance().width,
    coalengine::EngineUtils::Instance().height);

  

/*Imgui Init*/

  coalengine::RenderList::Instance().coal_imgui->InitImgui(wind.get());

/*Scheduler Init*/
  px_sched::Scheduler schd;
  px_sched::SchedulerParams s_params;
  schd.init(s_params);


  coalengine::Start();

  /*Mouse callbacks*/
  glfwSetCursorPosCallback(wind->window_, coalengine::RenderList::Instance().mouse_pos_callback);
  glfwSetMouseButtonCallback(wind->window_, coalengine::RenderList::Instance().mouse_click_callback);
  glfwSetKeyCallback(wind->window_, coalengine::RenderList::Instance().key_callback);

  px_sched::Sync s;
  px_sched::Sync s1;
  px_sched::Sync s2;
  px_sched::Sync s3;

  px_sched::Sync last;


  /* Loop until the user closes the window */
  while (!wind.get()->windowIsOpen())
  {
    coalengine::RenderList::Instance().ClearFrame();



    coalengine::EngineUtils::Instance().StartFrameTimer();

    /*Throw threads to process DP, Update and ECS*/
    schd.run(ProcessDisplayList, &s);
    last = s;
    if(coalengine::EngineUtils::Instance().play_update_)
    {
      schd.runAfter(s, coalengine::Update, &s1);
      last = s1;
    }

#ifndef SINGLE_CORE_ECS
    schd.runAfter(last, job_update_trs, &s2);
    schd.runAfter(last, job_update_semi, &s2);
    schd.runAfter(s2, job_lights, &s3);

    /* Render here */
    coalengine::RenderList::Instance().StartLoadTextures();
    render();
    schd.waitFor(s3);
    /*End of frame*/
    coalengine::ECS::Instance().SepareLights();
    coalengine::ECS::Instance().RenderSystem();
    job_create_delete();
#else
    coalengine::RenderList::Instance().StartLoadTextures();
    render();
    schd.waitFor(last);
    EngineUpdateSingleThread();


#endif




#ifdef JSON_MEASURE
    MTR_BEGIN("fourth", "EndFrameTimer");
#endif

    processInput(wind->window_, deltaTime, lastFrame);
    coalengine::RenderList::Instance().Swap();
    wind.get()->swap();
    wind.get()->processEvents();
    coalengine::EngineUtils::Instance().EndFrameTimer();

#ifdef JSON_MEASURE
    MTR_END("fourth", "EndFrameTimer");
#endif
  }
#ifdef JSON_MEASURE
  MTR_FINISH("background", "main", &time_trace);

  mtr_flush();
  mtr_shutdown();
#endif

  wind.get()->finish();
  coalengine::RenderList::Instance().coal_imgui->ImguiDestroy();
  return 0;
}

