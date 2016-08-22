#pragma once

#include "VMath.h"

namespace OBALFramework
{
  class Transform
  {
  public:
    Transform();
    Transform(FLOAT x, FLOAT y, FLOAT z, Quat rotation, FLOAT scalex, FLOAT scaley, FLOAT scalez);
    ~Transform();

    void UpdateTransform();

    Vec3 Position;
    Quat Rotation;
    Vec3 Scale;

    Vec3 LastPos = Vec3(0,0,0);
  };
}