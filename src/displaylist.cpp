#include <displaylist.h>
#include <RenderList.h>

#include "applypostprocess.h"
#include "clearscreen.h"
#include "drawentities.h"
#include "framebuffer.h"
#include "glad/glad.h"
#include "engineutils.h"
#include "cleardrawentities.h"
#include "drawcubemap.h"
#include "drawimgui.h"

coalengine::DisplayList::DisplayList()
{
  framebuffer_ = nullptr;
}

coalengine::DisplayList::DisplayList(DisplayList&& other)
{
  commands_ = std::move(other.commands_);

}

coalengine::DisplayList::~DisplayList()
{
}

coalengine::DisplayList& coalengine::DisplayList::operator=(DisplayList&& other)
{
  commands_.clear();
  commands_ = std::move(other.commands_);

  return *this;
}

void coalengine::DisplayList::addCommand(px::Mem<Command>&& command)
{
  commands_.push_back(std::move(command));
}

void coalengine::DisplayList::clearScreen()
{
  px::Mem<Command> clear_screen_;
  clear_screen_.alloc<ClearScreen>();
  commands_.push_back(std::move(clear_screen_));
}


void coalengine::DisplayList::DrawAllEntities()
{
  px::Mem<Command> draw_entities_;
  draw_entities_.alloc<DrawEntities>();
  commands_.push_back(std::move(draw_entities_));
}

void coalengine::DisplayList::ClearEntities()
{
  px::Mem<Command> clear_entities_;
  clear_entities_.alloc<ClearDrawEntities>();
  commands_.push_back(std::move(clear_entities_));
}
void coalengine::DisplayList::DrawSkyBox()
{
  px::Mem<Command> draw_cube_map;
  draw_cube_map.alloc<DrawCubeMap>();
  commands_.push_back(std::move(draw_cube_map));
}

void coalengine::DisplayList::DpImgui()
{
  px::Mem<Command> dpimgui;
  dpimgui.alloc<DrawImgui>();
  commands_.push_back(std::move(dpimgui));
}

void coalengine::DisplayList::ApplyPostProcess()
{
  px::Mem<Command> postprocess;
  postprocess.alloc<ApplyPostprocess>();
  commands_.push_back(std::move(postprocess));
}

void coalengine::DisplayList::RunAll()
{
  /*Generamos frame buffer con la textura*/

  if (framebuffer_ != nullptr)
  {
    switch (framebuffer_->type_)
    {
    case FrameBuffer::kNormalFB:
      RenderList::Instance().state_ = RenderList::kNormalRenderST;
      break;
    case FrameBuffer::kShadowFB:
      RenderList::Instance().state_ = RenderList::kShadowRenderST;
      RenderList::Instance().actual_s_light_++;
      break;
    case FrameBuffer::kHDR:
      RenderList::Instance().state_ = RenderList::kHDRRenderST;
      break;
    case FrameBuffer::kBloom:
      RenderList::Instance().state_ = RenderList::kBloomRenderST;
      break;
    case FrameBuffer::kDeferred:
      RenderList::Instance().state_ = RenderList::kDeferredRenderST;
      break;
    default:;
    }


    framebuffer_->FrameBufferGen();

  }

    for (unsigned int i = 0; i < commands_.size(); ++i)
    {
      commands_.at(i)->run();
    }
  if (framebuffer_ != nullptr) {

    framebuffer_->DrawFrameBuffer();
    if (EngineUtils::Instance().show_engine_front_ && (framebuffer_->type_ == FrameBuffer::kNormalFB ||
       framebuffer_->type_ == FrameBuffer::kBloom || framebuffer_->type_ == FrameBuffer::kDeferred
      || framebuffer_->type_ == FrameBuffer::kHDR))
    {
      glClearColor(1.0f,1.0f,1.0f,1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
  }
}

void coalengine::DisplayList::SetFramebuffer(const FrameBuffer* fbuffer)
{
  framebuffer_ = (FrameBuffer*)fbuffer;
}



