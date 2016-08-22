#include "Camera.h"
#include "Graphics.h"

namespace OBALFramework
{
  Camera::Camera()
  {
    OutputDebugStringA("CREATING: Camera\n");
    // Create a new TempTransform Object (This should be a component in the future)
    transform = new Transform();
    // Set this camera to the Graphics System's CurrentCamera
    GRAPHICS->CurrentCamera = this;
  }
  Camera::~Camera()
  {
    OutputDebugStringA("DESTRUCTING: Camera\n");
    // Detach from the Graphics System
    if (GRAPHICS->CurrentCamera == this)
      GRAPHICS->CurrentCamera = NULL;

    // Delete the tempTransform Object (Component)
    delete transform;
  }

  void Camera::Update()
  {
  }
}
