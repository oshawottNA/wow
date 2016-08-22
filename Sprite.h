#pragma once

#include <string>
#include "VMath.h"
#include "Transform.h"
#include "TextureSource.h"
//#include "GameObjectComposition.h"

namespace OBALFramework
{
  class Sprite// : public GameComponent
  {
  public:
    Sprite();
    Sprite(bool visible, bool visibilityculling, std::string spritename, std::string sourcename, 
            unsigned int startingrow, unsigned int startingframe,  Vec3 position, Quat rotation,
            Vec3 scale, Vec4 color);
    ~Sprite();

    void Initialize();
    void Draw(LPDIRECT3DDEVICE9 pDevice, FLOAT worldscale);

    
    Sprite * Next;
    Sprite * Prev;
    bool Visible = true;
    bool VisibilityCulling = false;   //Visibility Culling when this sprite is not being viewed by the camera
    std::string SpriteName;           //The name of this sprite
    std::string TextureName;          //Name of the sprite's SpriteSource asset
    TextureSource * SpriteTextureAsset;      //The Texture assset of the sprite
    RECT SourceRect;
    unsigned int CurrentRow = 0;      // The current row of the bitmap animation
    unsigned int CurrentFrame = 0;    // The current frame upon the current row of the bitmap animation
    Transform * transform;    //What Transform to use for this sprite
    D3DCOLOR SpriteColor;     //Blend color of this sprite
  };
}