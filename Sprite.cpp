#include "Sprite.h"
#include "Graphics.h"

namespace OBALFramework
{
  Sprite::Sprite()
  {
    OutputDebugStringA("Default Sprite Created\n");
    Visible = true;
    VisibilityCulling = false;
    SpriteName = "DefaultSpriteName";
    TextureName = "default64.bmp";
    CurrentRow = 0;
    CurrentFrame = 0;
    /// TEMPORARY
    transform = new Transform();
    SpriteColor = D3DCOLOR_RGBA(255, 255, 255, 255);
    SpriteTextureAsset = NULL;
    Initialize();
  }
  Sprite::Sprite(bool visible, bool visibilityculling, std::string spritename, std::string sourcename, 
                  unsigned int startingrow, unsigned int startingframe, Vec3 position, Quat rotation, 
                  Vec3 scale, Vec4 color)
  {
    Visible = visible;
    VisibilityCulling = visibilityculling;
    SpriteName = spritename;
    TextureName = sourcename;
    CurrentRow = startingrow;
    CurrentFrame = startingframe;
    transform = new Transform();
    (*transform).Position = position;
    (*transform).Rotation = rotation;
    (*transform).Scale = scale;
    SpriteColor = D3DCOLOR_RGBA((int)color.x, (int)color.y, (int)color.z, (int)color.w);
    SpriteTextureAsset = NULL;
    Initialize();
  }
  Sprite::~Sprite()
  {
    GRAPHICS->SpriteList.erase(this);
    delete transform;
  }


  void Sprite::Initialize()
  {
    OutputDebugStringA("Initializing Default Sprite\n");
    // Get the sprite's texture from the graphics engine
    SpriteTextureAsset = GRAPHICS->GetTextureSource(TextureName);

    // Get the dimensions of the spritesource (size has been deleted)
    ///size.x = (FLOAT)SpriteSourceAsset->SourceSizeX;
    ///size.y = (FLOAT)SpriteSourceAsset->SourceSizeY;

    if (SpriteTextureAsset != NULL)
    {
      OutputDebugStringA("Texture '");
      OutputDebugStringA(TextureName.c_str());
      OutputDebugStringA("' Found!!!\n");

      if (CurrentRow >= SpriteTextureAsset->NumberofRows)
        CurrentRow = 0;
      if (CurrentFrame >= SpriteTextureAsset->FramesPerRow)
        CurrentFrame = 0;
      
      SourceRect.top = CurrentRow * (LONG)SpriteTextureAsset->SourceSizeY;    // The initial row is the Current 
      SourceRect.left = CurrentFrame * (LONG)SpriteTextureAsset->SourceSizeX;
      SourceRect.right = SourceRect.left + (LONG)SpriteTextureAsset->SourceSizeX;
      SourceRect.bottom = SourceRect.top + (LONG)SpriteTextureAsset->SourceSizeY;
   }
    else
    {
      OutputDebugStringA("ERROR: SpriteSource with name '");
      OutputDebugStringA(TextureName.c_str());
      OutputDebugStringA("' was not found.\n");
    }
    
    
    // Push this sprite on the Graphics system's spritelist for drawing in the future
    GRAPHICS->SpriteList.push_back(this);
    //(*Owner).GetComponent();
  }
  void Sprite::Draw(LPDIRECT3DDEVICE9 pDevice, FLOAT worldscale)
  {
    D3DXMATRIX matRotateX;
    D3DXMATRIX matRotateY;
    D3DXMATRIX matRotateZ;
    D3DXMATRIX matScale;
    D3DXMATRIX matTranslate;
    D3DXMATRIX matTransform;

    D3DXMatrixRotationX(&matRotateX, D3DXToRadian(transform->Rotation.x));
    D3DXMatrixRotationY(&matRotateY, D3DXToRadian(transform->Rotation.y));
    D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(transform->Rotation.z));
    D3DXMatrixScaling(&matScale, transform->Scale.x, transform->Scale.y, transform->Scale.z);
    D3DXMatrixScaling(&matScale, worldscale, worldscale, worldscale);
    D3DXMatrixTranslation(&matTranslate, transform->Position.x, transform->Position.y, transform->Position.z);

    matTranslate = matRotateX * matRotateY * matRotateZ * matScale * matTranslate;

    pDevice->SetTransform(D3DTS_WORLD, &matTranslate);
    //Set the default texture for anything to be drawn
    pDevice->SetTexture(0, SpriteTextureAsset->Texture);

    //Thi is drawing the testSprite instead of the following square
    pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
  }

}
