#include "BackBufferSurfaceSprite.h"
#include "Graphics.h"

namespace OBALFramework
{
  BackBufferSurfaceSprite::BackBufferSurfaceSprite()
  {
    OutputDebugStringA("Default BackBufferSurfaceSprite Created\n");
    Visible = true;
    BackBufferSurfaceSpriteName = "DefaultBackBufferSurfaceSpriteName";
    SourceName = "default.bmp";
    CurrentRow = 0;
    CurrentFrame = 0;
    /// TEMPORARY
    transform = new Transform();
    (*transform).Position = Vec3(0.0f, 0.0f, 0.0f);
    Color = Vec4(0, 0, 0, 1);
    SpriteSourceAsset = NULL;
    Initialize();
  }
  BackBufferSurfaceSprite::BackBufferSurfaceSprite(bool visible, std::string spritename, std::string sourcename, 
                            unsigned int startingrow, unsigned int startingframe, Vec3 position, Quat rotation, 
                            Vec3 scale, Vec4 color)
  {
    Visible = visible;
    BackBufferSurfaceSpriteName = spritename;
    SourceName = sourcename;
    CurrentRow = startingrow;
    CurrentFrame = startingframe;
    transform = new Transform();
    (*transform).Position = position;
    (*transform).Rotation = rotation;
    (*transform).Scale = scale;
    Color = color;
    SpriteSourceAsset = NULL;
    Initialize();
  }
  BackBufferSurfaceSprite::~BackBufferSurfaceSprite()
  {
    GRAPHICS->BackBufferSurfaceSpriteList.erase(this);
    delete transform;
  }

  void BackBufferSurfaceSprite::Initialize()
  {
    OutputDebugStringA("Initializing Default BackBufferSurfaceSprite\n");
    // Get the sprite's texture from the graphics engine
    SpriteSourceAsset = GRAPHICS->GetSpriteSource(SourceName);

    // Get the dimensions of the spritesource (size has been deleted)
    ///size.x = (FLOAT)SpriteSourceAsset->SourceSizeX;
    ///size.y = (FLOAT)SpriteSourceAsset->SourceSizeY;

    if (SpriteSourceAsset != NULL)
    {
      OutputDebugStringA("SpriteSource ");
      OutputDebugStringA(SourceName.c_str());
      OutputDebugStringA(" Found!!!\n");

      if (CurrentRow >= SpriteSourceAsset->NumberofRows)
        CurrentRow = 0;
      if (CurrentFrame >= SpriteSourceAsset->FramesPerRow)
        CurrentFrame = 0;

      CurrentRow = 0;
      CurrentFrame = 0;

      BackBufferSourceRect.top = CurrentRow * (LONG)SpriteSourceAsset->SourceSizeY;    // The initial row is the Current 
      BackBufferSourceRect.left = CurrentFrame * (LONG)SpriteSourceAsset->SourceSizeX;
      BackBufferSourceRect.right = BackBufferSourceRect.left + (LONG)SpriteSourceAsset->SourceSizeX;
      BackBufferSourceRect.bottom = BackBufferSourceRect.top + (LONG)SpriteSourceAsset->SourceSizeY;
    }
    else
    {
      OutputDebugStringA("ERROR: SpriteSource with name '");
      OutputDebugStringA(SourceName.c_str());
      OutputDebugStringA("' was not found.\n");
    }


    // Push this sprite on the Graphics system's spritelist for drawing in the future
    GRAPHICS->BackBufferSurfaceSpriteList.push_back(this);
    //(*Owner).GetComponent();
  }


}
