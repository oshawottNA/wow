#include "HUDSprite.h"
#include "Graphics.h"

namespace OBALFramework
{
  HUDSprite::HUDSprite()
  {
    OutputDebugStringA("Default HUDSprite Created\n");
    Visible = true;
    VisibilityCulling = false;
    HUDSpriteName = "DefaultHUDSpriteName";
    TextureName = "square.png";
    CurrentRow = 0;
    CurrentFrame = 0;
    /// TEMPORARY
    transform = new Transform();
    HUDSpriteColor = D3DCOLOR_RGBA(255, 255, 255, 255);
    HUDSpriteTextureAsset = NULL;
    Initialize();
  }
  HUDSprite::HUDSprite(bool visible, bool visibilityculling, std::string spritename, std::string sourcename,
    unsigned int startingrow, unsigned int startingframe, Vec3 position, Quat rotation,
    Vec3 scale, Vec4 color)
  {
    Visible = visible;
    VisibilityCulling = visibilityculling;
    HUDSpriteName = spritename;
    TextureName = sourcename;
    CurrentRow = startingrow;
    CurrentFrame = startingframe;
    transform = new Transform();
    (*transform).Position = position;
    (*transform).Rotation = rotation;
    (*transform).Scale = scale;
    HUDSpriteColor = D3DCOLOR_RGBA((int)color.x, (int)color.y, (int)color.z, (int)color.w);
    HUDSpriteTextureAsset = NULL;
    Initialize();
  }
  HUDSprite::~HUDSprite()
  {
    GRAPHICS->HUDSpriteList.erase(this);
    delete transform;
  }


  void HUDSprite::Initialize()
  {
    OutputDebugStringA("Initializing Default HUDSprite\n");
    // Get the sprite's texture from the graphics engine
    HUDSpriteTextureAsset = GRAPHICS->GetTextureSource(TextureName);

    // Get the dimensions of the spritesource (size has been deleted)
    ///size.x = (FLOAT)SpriteSourceAsset->SourceSizeX;
    ///size.y = (FLOAT)SpriteSourceAsset->SourceSizeY;

    if (HUDSpriteTextureAsset != NULL)
    {
      OutputDebugStringA("Texture '");
      OutputDebugStringA(TextureName.c_str());
      OutputDebugStringA("' Found!!!\n");

      if (CurrentRow >= HUDSpriteTextureAsset->NumberofRows)
        CurrentRow = 0;
      if (CurrentFrame >= HUDSpriteTextureAsset->FramesPerRow)
        CurrentFrame = 0;

      SourceRect.top = CurrentRow * (LONG)HUDSpriteTextureAsset->SourceSizeY;    // The initial row is the Current 
      SourceRect.left = CurrentFrame * (LONG)HUDSpriteTextureAsset->SourceSizeX;
      SourceRect.right = SourceRect.left + (LONG)HUDSpriteTextureAsset->SourceSizeX;
      SourceRect.bottom = SourceRect.top + (LONG)HUDSpriteTextureAsset->SourceSizeY;
    }
    else
    {
      OutputDebugStringA("ERROR: SpriteSource with name '");
      OutputDebugStringA(TextureName.c_str());
      OutputDebugStringA("' was not found.\n");
    }


    // Push this sprite on the Graphics system's spritelist for drawing in the future
    GRAPHICS->HUDSpriteList.push_back(this);
    //(*Owner).GetComponent();
  }
  void HUDSprite::Draw(LPD3DXSPRITE pSprite)
  {
    D3DXMATRIX rot;
    D3DXMATRIX scale;
    D3DXMATRIX trans;
    D3DXMATRIX world;

    //Rotate it - only need z rotation
    D3DXMatrixRotationYawPitchRoll(&rot,
      transform->Rotation.x,
      transform->Rotation.y,
      transform->Rotation.z);

    //Scale it - no z scale
    D3DXMatrixScaling(&scale,
      transform->Scale.x,
      transform->Scale.y,
      1.0f);

    //Translate it - zero out z translation
    D3DXMatrixTranslation(&trans,
      transform->Position.x,
      transform->Position.y,
      transform->Position.z);
    //Concatenate
    world = rot*scale*trans;

    //pSprite->SetTransform(&world);

    //Vec3 spritecenter = Vec3((FLOAT)it->SpriteTextureAsset->OriginX, (FLOAT)it->SpriteTextureAsset->OriginY, (FLOAT)it->transform->Position.z);

    HRESULT hr = pSprite->Draw(HUDSpriteTextureAsset->Texture, &SourceRect, &transform->Position,
      &transform->Position, HUDSpriteColor);

    // Check to see if drawing was successful
    if (!SUCCEEDED(hr))
      OutputDebugStringA("HUDSprite Draw FAILED!!!");
    //else OutputDebugStringA("Sprite Draw SUCCEEDED!!!");
  }

}
