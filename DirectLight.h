#pragma once

#include "Graphics.h"

namespace OBALFramework
{
  class DirectLight
  {
  public:
    DirectLight();
    ~DirectLight();

    int index;
    D3DLIGHT9 Light
  };
}