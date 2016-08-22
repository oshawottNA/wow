#include "Transform.h"

namespace OBALFramework
{
  Transform::Transform()
  {
    OutputDebugStringA("CREATING: TempTransform\n");
    Position = Vec3(0,0,0);
    Rotation = Quat(0.0f, 0.0f, 0.0f, 0.0f);
    Scale = Vec3(1.0f,1.0f,1.0f);
  }
  Transform::Transform(FLOAT x, FLOAT y, FLOAT z, Quat rotation, FLOAT scalex, FLOAT scaley, FLOAT scalez)
  {
    OutputDebugStringA("CREATING: TempTransform\n");
    Position = Vec3(x, y, z);
    Rotation = rotation;
    Scale = Vec3(scalex, scaley, scalez);

  }
  Transform::~Transform()
  {
    OutputDebugStringA("DESTRUCTING: TempTransform\n");
  }


  void Transform::UpdateTransform()
  {
    LastPos = Position;
  }
}
