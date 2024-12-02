/**
* @author Alejandro SG y Anibal SG
* @file drawentities.h
* @brief file logic for draw entities
*/

#ifndef __GENERATE_IRRADIANCE_MAP__
#define __GENERATE_IRRADIANCE_MAP__ 1

#include <string>
#include <glad/glad.h>

#include "Command.h"

namespace coalengine
{
  /** @brief Class command for Draw all geometries.
  *
  */
  class GenerateIrradianceMap : public Command
  {
  public:
    GenerateIrradianceMap();
    ~GenerateIrradianceMap();
    /** @brief Function for run the command.
    *
    */
    void run() override;
  };
}

#endif

