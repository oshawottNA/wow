#pragma once

#include "Transform.h"

namespace OBALFramework
{
  class Camera
  {
  public:
    Camera();
    ~Camera();

    void Update();

    Transform * transform;
  };
}