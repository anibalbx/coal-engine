#include "applypostprocess.h"
#include "framebuffer.h"
#include "renderlist.h"

coalengine::ApplyPostprocess::ApplyPostprocess()
{
}

coalengine::ApplyPostprocess::~ApplyPostprocess()
{
}

void coalengine::ApplyPostprocess::run()
{
  RenderList& RL = RenderList::Instance();

  for (int i = 0; i < RL.post_process_list_.size(); ++i)
  {
    RL.post_process_list_.at(i)->RunPostPro(i);
  }
}
