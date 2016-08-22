#include "Precompiled.h"
#include "Graphics.h"
#include "VertexTypes.h"
#include "FilePath.h"

// Testing Messaging
#include "MessageRecipientList.h"
#include "WindowsSystem.h"  //For Messaging stuff
//#include "OBALCore.h"  //For CORE->...
#include <sstream>

#define GRAY_COLOR	0x00bfbfbf

#define Ortho_Cam 1   // Change to 1 if the camera should be orthographic, which isn't fully implemented yet (otherwise Perspective)  
#define CUSTOMVERTEX_Use 0  //Don't change.  This is used to test textures, which isn't fully implemented yet.

namespace OBALFramework
{
  //Our global pointer to Graphics.
  Graphics* GRAPHICS = NULL;


  /// *****CONSTRUCTOR/DESTRUCTOR*****
  Graphics::Graphics()
  {
    OutputDebugStringA("CONSTRUCTING: Graphics\n");
    //These will be set later
    ScreenWidth = 0;    // Width of the Window
    ScreenHeight = 0;   // Height of the Window

    FillMode = D3DFILL_SOLID;

    pD3D = NULL;
    pDevice = NULL;
    v_buffer = NULL;

    // Set World Scale if Camera is Orthographic
    if (Ortho_Cam)
      WorldScale = WorldScale;
    else
      WorldScale = 1.0f;

    //Initialize Camera variables
    CamPosition = Vec3(0.0f, 0.0f, 0.0f);    // the camera position
    CamLookAt = Vec3(0.0f, 0.0f, 0.0f);        // the look-at position
    CamUpDirection = Vec3(0.0f, 1.0f, 0.0f);   // the camera UP direction

    GRAPHICS = this;
  }
  Graphics::~Graphics()
  {
    OutputDebugStringA("DESTRUCTING: Graphics\n");
    // clean up DirectX and COM
    ShutDownD3D();
    ///////////////////////

    //Release all the texture in the texture
    for (TextureMap::iterator it = TextureSources.begin(); it != TextureSources.end(); ++it)
    {
      if (it->second->Texture)
      {
        it->second->Texture->Release();
        it->second->Texture = NULL;
        delete it->second;
      }
      else
        OutputDebugStringA("Attempted to release a texture that was already NULL.\n");
    }
    
    //Delete all dynamically allocated the SpriteSources
    for (SurfaceMap::iterator it = SpriteSources.begin(); it != SpriteSources.end(); ++it)
    {
      if (it->second->SurfaceSource)
      {
        it->second->SurfaceSource->Release();
        it->second->SurfaceSource = NULL;
        delete it->second;
      }
      else
        OutputDebugStringA("Attempted to release a surface that was already NULL.\n");
    }

    /*
    //Spritesources use new
    ObjectLinkList<Sprite>::iterator it = SpriteList.begin();
    for (; it != SpriteList.end(); ++it)
    {
      SpriteList.erase(it);
      //delete it;
    }
    */
    //SpriteSources.clear();
    ///////////////////////

    delete CurrentCamera;
    //CurrentCamera = NULL;
    delete testDefaultText;
    delete testHUDSprite;
    delete testSprite2;
    delete testSprite;
    //delete testBackBufferSurfaceSprite;
  }


  /// *****INITIALIZATION*****
  void Graphics::SetWindowProperties(HWND hWnd, int screenWidth, int screenHeight)
  {
    OutputDebugStringA("SETTING WINDOW PROPERTIES: Graphics\n");
    HWnd = hWnd;
    ScreenWidth = screenWidth;
    ScreenHeight = screenHeight;
  }


  //Initializes Direct3D
  void Graphics::Initialize()
  {
    OutputDebugStringA("INITIALIZING: Graphics\n");

    //Tell the CoreEngine to Register the windows system into the Quit message Recipient Vector
    //This way, the windows system will ONLY get CharacterKey messages
    OutputDebugStringA("REGISTERING FOR HeldKey MESSAGE: Graphics\n");
    MSGRECIPIENTS->AddRecipient(this, Mid::HeldKey);


    /// Create D3D Interface
    pD3D = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface
    
    /// Get Info from the Video Adapter (Currently, this just prints the supported
    /// dimensions and refresh rates to the debug console)
    GetAdapterInfo();

    /// Getting/Setting Device info
    D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information
    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use

    d3dpp.Windowed = true;    // program windowed, not fullscreen  -- Leaving this TRUE with fullscreen settings will only make a boarderless window with the given width and height
    d3dpp.hDeviceWindow = HWnd;    // set the game window to be used by Direct3D
                                   //...for fullscreen follows
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;  //D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit
    d3dpp.BackBufferCount = 1;                // specify the number of back buffers being used
    d3dpp.BackBufferWidth = ScreenWidth;      // set the width of the buffer
    d3dpp.BackBufferHeight = ScreenHeight;    // set the height of the buffer
    //  Adding Z_Buffer for depth
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    /// Create D3D Device
    // create a device class using this information and information from the d3dpp stuct
    pD3D->CreateDevice(D3DADAPTER_DEFAULT,
                       D3DDEVTYPE_HAL,
                       HWnd,
                       D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                       &d3dpp,
                       &pDevice);


    /// Initialize Graphics (define verticies and vertex/index/etc. buffers)
    /// and load all surfaces and textures.
    LoadAssets();

    
    // Create the Sprite Object so sprites can be drawn
    HRESULT hr = D3DXCreateSprite(pDevice, &pSprite);
    if (!SUCCEEDED(hr))
    {
      //std::cerr << "Error getting buffer from swap chain: " << std::hex << res << std::endl;
      OutputDebugStringA("Error making Sprite Object.\n");
    }
    else
    {
      OutputDebugStringA("Sprite Object created.\n");
    }

    // TEST OBJECTS BEFORE OBJECT COMPOSITION WAS IMPLEMENTED
    //testBackBufferSurfaceSprite = new BackBufferSurfaceSprite();
    testSprite = new Sprite();
    testSprite2 = new Sprite();
    testSprite2->transform->Position.x = 1.0f;
    // If the Camera is Orthographic, scale by the orthographic scale
#if (Ortho_Cam)
    testSprite2->transform->Position.x *= OrthoScale;
#endif
    testHUDSprite = new HUDSprite();
    testDefaultText = new DefaultText();


    /// Set Render States (Set Lighting, Culling, Z-Buffer, etc.)
    // Include Lighting?
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn off the 3D lighting
                                                      // create material
    D3DMATERIAL9 mtrl;
    ZeroMemory(&mtrl, sizeof(mtrl));
    mtrl.Ambient.r = 0.75f;
    mtrl.Ambient.g = 0.75f;
    mtrl.Ambient.b = 0.75f;
    mtrl.Ambient.a = 0.0f;
    pDevice->SetMaterial(&mtrl);
    pDevice->SetRenderState(D3DRS_AMBIENT, GRAY_COLOR);

    // Anti-Aliasing
    //pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

    // This is for FOG, which we cannot use because it only works with a perspective camera
    /*
    float thing = 0.1f;
    float thang = 0.9f;
    float density = 0.1f;   // For exponential modes
    pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    // Set the fog color.
    pDevice->SetRenderState(D3DRS_FOGCOLOR, 0x000000bf);
    pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
    //pDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&thing);
    //pDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&thang);
    pDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)&density);
    */

    //pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // render both sides of the triangles
    // Turn on Z-Buffer for depth
    pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
    // Turn on Color Blending
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);   // set the Blend operation
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);   // 
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);   // 


    ScreenSize = Vec2((float)ScreenWidth, (float)ScreenHeight);

    // Create a Camera object
    // NOTE: This isn't actually the camera the DirectX uses.  Its basically just a temporary transform object.
    CurrentCamera = new Camera();
    CurrentCamera->transform->Position = Vec3(0.0f, 0.0f, 15.0f);


    pFont = NULL;
    //Create a now font object.  This font is created and handled by DirectX.
    D3DXCreateFontA(pDevice, 24, 0, FW_NORMAL, 0, false, ANSI_CHARSET, OUT_OUTLINE_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, "Ariel", &pFont);
    ///Creating Font Object Parameters: 
    ///DirectX device, font height size, font width size, bold weight, mipmaps, italics(?), character set (ANSI or Unicode), outputprecision, match font's desired quality with font's default quality, font's pitch and family indexes, name of desired font (that is installed on system), output address for font object.

    
#if(Ortho_Cam)
    FLOAT scale = (FLOAT)OrthoScale;
    //D3DXMatrixScaling(&OrthoMapTransform, scale, scale, scale);
    //Mat4 mapscale;
    //Mat4Scale(&OrthoMapTransform, scale, scale, scale);
    //Mat4Concatenate();
#endif

  } // END initD3D()


  void Graphics::GetAdapterInfo()
  {
    OutputDebugStringA("GETTING VIDEO ADAPTER INFO: Graphics\n");
    /// Getting Adapter info
    //This is where we get this computers video adapter information 
    //(This info is used for things like knowing which resolutions this computer can support, etc.)
    D3DADAPTER_IDENTIFIER9 ident;
    pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &ident);
    OutputDebugStringA("\n\n*** Adapter Details ***\n");
    OutputDebugStringA(ident.Description);
    OutputDebugStringA("\n");
    OutputDebugStringA(ident.DeviceName);
    OutputDebugStringA("\n");
    OutputDebugStringA(ident.Driver);
    OutputDebugStringA("\n\n--------------------\n\n");

    // collect the modes this adapter has
    UINT numModes = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);

    // loop through and print the info for each mode
    for (UINT i = 0; i < numModes; ++i)
    {
      D3DDISPLAYMODE mode;    // Define the D3DDISPLAYMODE structure
      char modeString[255];   // Temporary char array
                              // Get the displaymode struct for this adapter mode
      pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,
                             D3DFMT_X8R8G8B8,
                             i,
                             &mode);

      
      sprintf_s(modeString, "Width = %d", mode.Width);
      OutputDebugStringA(modeString);
      OutputDebugStringA("\n");
      sprintf_s(modeString, "Height = %d", mode.Height);
      OutputDebugStringA(modeString);
      OutputDebugStringA("\n");
      // Output the Refresh Rate
      sprintf_s(modeString, "Refresh Rate = %d", mode.RefreshRate);
      OutputDebugStringA(modeString);
      OutputDebugStringA("\n\n--------------------\n\n\n");
    }
  }


  void Graphics::init_graphics(void)
  {
    
#if(CUSTOMVERTEX_Use)
    // create the vertices using the CUSTOMVERTEX struct
    // Makes a Square
    CUSTOMVERTEX t_verts[] =
    {
      { -3.0f, 3.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 0, 255), },
      { 3.0f, 3.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 255, 0), },
      { -3.0f, -3.0f, 0.0f, D3DCOLOR_ARGB(255, 255, 0, 0), },
      { 3.0f, -3.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 255, 255), },
    };

    // create a vertex buffer interface called v_buffer (Pyramid)
    /// For Pyramid
    pDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
      0,
      CUSTOM_FVF,
      D3DPOOL_MANAGED,
      &v_buffer,
      NULL);
#else
    // create the vertices using the CUSTOMVERTEX struct
    // Makes a Square
    VERTEX2D t_verts[] =
    {
      { -0.5f, 0.5f, 0.0f, 0.0f, 0.0f  },
      { 0.5f, 0.5f, 0.0f, 1.0f, 0.0f },
      { -0.5f, -0.5f, 0.0f, 0.0f, 1.0f },
      { 0.5f, -0.5f, 0.0f, 1.0f, 1.0f },
    };


    // create a vertex buffer interface called v_buffer (Pyramid)
    /// For Square
    pDevice->CreateVertexBuffer(4 * sizeof(VERTEX2D),
                                0,
                                VERTEX2D_FVF,
                                D3DPOOL_MANAGED,
                                &v_buffer,
                                NULL);
#endif
    


    VOID* pVoid;    // a void pointer

    // lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);

    memcpy(pVoid, t_verts, sizeof(t_verts)); //t_vert

    //Then, unlock v_buffer
    v_buffer->Unlock();
  }


  // TESTING MESSAGING
  void Graphics::ThrowMessage(Message * message)
  {
    OutputDebugStringA("MESSAGE THROWN TO THIS RECIPIENT: Graphics\n");
    switch (message->MessageId)
    {
      OutputDebugStringA("FINDING MESSAGE CASE: Graphics\n");
      // The user has pressed a (letter/number) key, we may respond by creating
      // a specific object based on the key pressed.
      case Mid::HeldKey:
      {
        MessageHeldKey * held = (MessageHeldKey*)message;
        if (held->key == 'W')
        {
          OutputDebugStringA("'w' BUTTON PRESSED : TempGameLogic\n");
          CurrentCamera->transform->Position.y += 1;
        }
        if (held->key == 'S')
        {
          OutputDebugStringA("'s' BUTTON PRESSED : Graphics\n");
          CurrentCamera->transform->Position.y -= 1;
        }
        if (held->key == 'A')
        {
          OutputDebugStringA("'a' BUTTON PRESSED : Graphics\n");
          CurrentCamera->transform->Position.x -= 1;
        }
        if (held->key == 'D')
        {
          OutputDebugStringA("'d' BUTTON PRESSED : Graphics\n");
          CurrentCamera->transform->Position.x += 1;
        }
        if (held->key == VK_CONTROL)
        {
          OutputDebugStringA("'Ctrl' BUTTON PRESSED : Graphics\n");
          CurrentCamera->transform->Position.z += 1;
        }
        if (held->key == VK_SHIFT)
        {
          OutputDebugStringA("'Shift' BUTTON PRESSED : Graphics\n");
          CurrentCamera->transform->Position.z -= 1;
        }
        // Change FillMode
        if (held->key == 'F')
        {
          OutputDebugStringA("'F' BUTTON PRESSED : Graphics\n");
          if (FillMode == D3DFILL_SOLID)
          {
            FillMode = D3DFILL_WIREFRAME; 
            OutputDebugStringA("WIREFRAME\n");
            pDevice->SetRenderState(D3DRS_FILLMODE, FillMode);
          }
          else if (FillMode == D3DFILL_WIREFRAME)
          {
            FillMode = D3DFILL_SOLID;
            OutputDebugStringA("SOLID\n");
            pDevice->SetRenderState(D3DRS_FILLMODE, FillMode);
          }
        }
      }
    }
  }


  /// *****UPDATING*****
  void Graphics::Update(float dt)
  {
    /// CLEAR Previous Buffers
    // clear the window to a deep blue
    pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), /*1.0f in tutorial*/ 0.0f, 0);
    // clear the Z-Buffer
    pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);


    //Begin the scene.  If successful, draw the next frame
    if (SUCCEEDED(pDevice->BeginScene()))
    {

      // Draw Everything
      DrawWorld();
      // *END the 3D scene
      pDevice->EndScene();
    }

    // Displays the created frame
    pDevice->Present(NULL, NULL, NULL, NULL);

  } // END Update()
  

  void Graphics::DrawWorld(void)
  {

#if(CUSTOMVERTEX_Use)
    // select which vertex format we are using
    pDevice->SetFVF(CUSTOM_FVF);
#else
    // select which vertex format we are using
    pDevice->SetFVF(VERTEX2D_FVF);
#endif

    // Get and Set the Updated camera position, lookat, and such
    SetCameraMatrices();


    /// OBJECT MANIPULATION AND SHOWING IN WINDOW
    // ROTATES THE Square
    {
      D3DXMATRIX matRotateY;    // a matrix to store the rotation for each triangle
      static float index = 0.0f; index += 0.05f; // an ever-increasing float value

      D3DXMatrixRotationY(&matRotateY, index);    // the front side
      //Delete the following comment to make the square rotate automatically
      //pDevice->SetTransform(D3DTS_WORLD, &matRotateY);
    }


    /// VERTEX BUFFER
#if(CUSTOMVERTEX_Use)
    // select the vertex buffer to display
    pDevice->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
#else
    // select the vertex buffer to display
    pDevice->SetStreamSource(0, v_buffer, 0, sizeof(VERTEX2D));
#endif
    

    // Go through and draw all text and sprite objects
    DrawDefaultTexts();
    DrawBackBufferSurfaceSprites();
    DrawHUDSprites();
    DrawSprites();


    //pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


  } // END DrawWorld()


  void Graphics::SetCameraMatrices(void)
  {
    // get the Camera's current transformation and apply it to CamPosition
    float cameraX = CurrentCamera->transform->Position.x;
    float cameraY = CurrentCamera->transform->Position.y;
    float cameraZ = CurrentCamera->transform->Position.z;
    CamPosition = Vec3(cameraX,cameraY, cameraZ);
    
    /// VIEW (CAMERA) TRANSFORM
    // set the view transform
    D3DXMATRIX matView;
    D3DXMatrixLookAtRH(&matView,
                       &CamPosition,     //&D3DXVECTOR3(0.0f, -1.0f, 15.0f),   // the camera position
                       &CamLookAt,       //&D3DXVECTOR3(0.0f, 0.0f, 0.0f),     // the look-at position
                       &CamUpDirection); //&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
    //Apply the new Camera Properties for this update to the ViewMatrix
    ViewMatrix = matView;


    /// PROJECTION (CAMERA LENS/VIEWING PROPERTIES) TRANSFORM
    // set the projection transform
    D3DXMATRIX matProjection;

#if(Ortho_Cam)
    D3DXMatrixOrthoRH(&matProjection,
                      ScreenSize.x,     // the Screen width
                      ScreenSize.y,     // the Screen Height
                      0.5f,             // the near view-plane
                      1000.0f);          // the far view-plane
#else
    D3DXMatrixPerspectiveFovRH(&matProjection,
                               D3DXToRadian(45),    // the horizontal field of view
                               ScreenSize.x / ScreenSize.y, // aspect ratio (Screen width / Screen Height)
                               0.5f,       // the near view-plane
                               1000.0f);    // the far view-plane
#endif

    //Apply the new Camera Properties for this update to the ViewMatrix
    ProjMatrix = matProjection;


    /// SET TRANSFORMS TO CAMERA
    ///THESE SetTransform() FUNCTIONS WILL BE HANDLED WHEN TRANSFORM COMPONENTS ARE CREATED****************************
    pDevice->SetTransform(D3DTS_VIEW, &ViewMatrix);    // set the view transform to matView
    pDevice->SetTransform(D3DTS_PROJECTION, &ProjMatrix);     // set the projection


    //Store the view projection matrix
    ViewProjMatrix = ViewMatrix * ProjMatrix;
    

    // Show camera debug info on screen
    Debug_TrackCamera();

  } // END SetCameraMatrices()


  void Graphics::DrawDefaultTexts()
  {
    // Draw all active DefaultText objects
    ObjectLinkList<DefaultText>::iterator it = DirectTextVec.begin();
    for (; it != DirectTextVec.end(); ++it)
    {
      it->Draw(&pFont);
    }

  }
  void Graphics::DrawHUDSprites()
  {
    if (pSprite)
    {
      D3DXMATRIX world;
      D3DXMatrixIdentity(&world);
      //pDevice->GetTransform(D3DTS_WORLD, &worldMat);
      pSprite->SetWorldViewRH(&world, &ViewMatrix);

      // Prepare the device for drawing sprites.
      // Sort batched sprites by their textures | have them face the camera always
      pSprite->Begin(D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_ALPHABLEND);


      //Iterate through the link list of sprite objects and draw them all
      ObjectLinkList<HUDSprite>::iterator it = HUDSpriteList.begin();
      for (; it != HUDSpriteList.end(); ++it)
        it->Draw(pSprite);

      //Force all batched sprites to submitted to the device.
      // Then, restore the device state to how it was before Begin() was called 
      //  (unless D3DXSPRITE_DONOTSAVESTATE was a flag in begin())
      pSprite->End();

    }
    else
    {
      OutputDebugStringA("pSprite was not created.\n");
      OutputDebugStringA("HUDSprite objects cannot be drawn.\n");
    }
  }
  void Graphics::DrawSprites()
  {
    if (pDevice)
    {
      // Each object being drawn needs to scaled if the camera is in Orthographic mode (Ortho_Cam = 1)
      FLOAT worldscale = 0.0f;

      if (Ortho_Cam == 1)
        worldscale = (float)OrthoScale;
      else
        worldscale = 1.0f;

      pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // render both sides of the triangles

      // DO STUFF HERE
      //Iterate through the link list of sprite objects and draw them all
      ObjectLinkList<Sprite>::iterator it = SpriteList.begin();
      for (; it != SpriteList.end(); ++it)
        it->Draw(pDevice, worldscale);

      pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);    // render both sides of the triangles
    }
    else
    {
      OutputDebugStringA("pDevice NULL.\n");
      OutputDebugStringA("Sprite objects cannot be drawn.\n");
    }
  }
  void Graphics::DrawBackBufferSurfaceSprites()
  {
    if (pDevice)
    {
      // Get a surface of the backbuffer
      IDirect3DSurface9* backbuffer = NULL;
      pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

      //Iterate through the link list of sprite and draw them all
      //TODO: Need Visibility to cull off screen sprites
      ObjectLinkList<BackBufferSurfaceSprite>::iterator it = BackBufferSurfaceSpriteList.begin();
      for (; it != BackBufferSurfaceSpriteList.end(); ++it)
      {
        RECT destRect;
        destRect.left = (LONG)it->transform->Position.x;
        destRect.top = (LONG)it->transform->Position.y;
        destRect.bottom = destRect.top + it->SpriteSourceAsset->SourceSizeX;
        destRect.right = destRect.left + it->SpriteSourceAsset->SourceSizeY;

        //OutputDebugStringA("Drawing Default Sprite\n");
        pDevice->StretchRect(it->SpriteSourceAsset->SurfaceSource,
                             &it->BackBufferSourceRect,
                             backbuffer,
                             &destRect,
                             D3DTEXF_NONE);
        
        //OutputDebugStringA("Default Sprite Created\n");

        //if (it->SpriteSourceAsset->SurfaceSource != NULL)
          //OutputDebugStringA("DEFAULT SurfaceSource is something");
        //else
          //OutputDebugStringA("DEFAULT SurfaceSource is NOT something");
      }
    }
  }


  //Load all the textures we need.
  void Graphics::LoadAssets()
  {
    // Initialize Graphics (Vertices and what not)
    init_graphics();

    // Load all textures
    LoadTexture("assets\\default.bmp",1 ,1 ,1);
    LoadTexture("assets\\default64.bmp", 1, 1, 1);
    LoadTexture("assets\\Circle.png", 1, 1, 1);
    LoadTexture("assets\\CircleBordered.png", 1, 1, 1);
    LoadTexture("assets\\Square.png", 1, 1, 1);
    LoadTexture("assets\\SquareBordered.png", 1, 1, 1);
    LoadTexture("assets\\woodfloor.bmp", 1, 1, 1);
    LoadSpriteSource("assets\\default.bmp", 1, 1, 1);

  } // END LoadAssets()

  void Graphics::LoadTexture(const std::string& filename, unsigned int framesperrow, unsigned int numofrows, 
                              unsigned int framerate)
  {
    FilePath texturefile(filename);

    //filename.c_str()
    //First, check to see if the texture being loaded has already been loaded
    if (TextureSources.find(texturefile.FullPath.c_str()) == TextureSources.end())
    {
      //Create a new texture object
      IDirect3DTexture9* newTexture = NULL;
      D3DXIMAGE_INFO imageInfo;

      //Use D3DX to load the texture into newTexture
      if (SUCCEEDED(D3DXGetImageInfoFromFileA(texturefile.FullPath.c_str(), &imageInfo)))
      {
        //Use D3DX to load the texture into newTexture
        if (SUCCEEDED(D3DXCreateTextureFromFileA(pDevice, texturefile.FullPath.c_str(), &newTexture)))
        {
          OutputDebugStringA("Texture '");
          OutputDebugStringA(texturefile.FileName.c_str());
          OutputDebugStringA("' has been loaded.\n");

          TextureSource * newTextureSource = new TextureSource(texturefile.FileName.c_str(), newTexture, framesperrow, numofrows, framerate, imageInfo.Width, imageInfo.Height);

          // Add the new texture to the texture map container
          TextureSources[texturefile.FileName.c_str()] = newTextureSource;
        }
        else
        {
          OutputDebugStringA("Failed to load texture: '");
          OutputDebugStringA(texturefile.FileName.c_str());
          OutputDebugStringA("' in File Path: '");
          OutputDebugStringA(texturefile.FullPath.c_str());
          OutputDebugStringA("'\n");
        }
      }
      else
      {
        OutputDebugStringA("Could not get necessary info from image: '");
        OutputDebugStringA(texturefile.FileName.c_str());
        OutputDebugStringA("' with File Path: '");
        OutputDebugStringA(texturefile.FullPath.c_str());
        OutputDebugStringA("' for texture.   (Image may not exist)\n");
      }
    }
    else
    {
      OutputDebugStringA("Texture with name '");
      OutputDebugStringA(texturefile.FileName.c_str());
      OutputDebugStringA("' with File Path: '");
      OutputDebugStringA(texturefile.FullPath.c_str());
      OutputDebugStringA("' has already been loaded.\n");
    }

    //pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    //pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

  } // END LoadTexture()
  void Graphics::LoadSpriteSource(const std::string& filename, unsigned int framesperrow, unsigned int numofrows, 
                                    unsigned int framerate)
  {
    //First, check to see if the texture being loaded has already been loaded
    if (SpriteSources.find(filename.c_str()) == SpriteSources.end())
    {
      //Create a new Surface object
      IDirect3DSurface9* newSurface = NULL;
      D3DXIMAGE_INFO imageInfo;

      //Use D3DX to load the texture into newTexture
      if (SUCCEEDED(D3DXGetImageInfoFromFileA(filename.c_str(), &imageInfo)))
      {
        if (SUCCEEDED(pDevice->CreateOffscreenPlainSurface(imageInfo.Width,
                                                           imageInfo.Height,
                                                           D3DFMT_X8R8G8B8,
                                                           D3DPOOL_DEFAULT,
                                                           &newSurface,
                                                           NULL)))
        {
          if (SUCCEEDED(D3DXLoadSurfaceFromFileA(newSurface,        // The surface to load to bitmap onto
                                                 NULL,              // Pointer to a PALEETTEENTRY for 256-color bitmaps
                                                 NULL,              // Pointer to RECT that reps area of surface that bitmap should be loaded to
                                                 filename.c_str(),  // String reping file name of bitmap to load
                                                 NULL,              // Pointer to RECT that rps area of source bitmap to be loaded into surface
                                                 D3DX_DEFAULT,      // D3DX_FILTER that specifies type of filtering to be applied
                                                 0,                 // D3DCOLOR format of the color that should be used for transparency (0 = default)
                                                 NULL)))            // Pointer to a D3DXIMAGE_INFO struct.  Holds info about source bitmap file (width, height, bit depth)
          {

          }
          OutputDebugStringA("Image for SpriteSource '");
          OutputDebugStringA(filename.c_str());
          OutputDebugStringA("' has been loaded.\n");

          SpriteSource * newSpriteSource = new SpriteSource(filename.c_str(), newSurface, framesperrow, numofrows, framerate, imageInfo.Width, imageInfo.Height);

          // Add the new SpriteSource to the texture map container
          SpriteSources[filename.c_str()] = newSpriteSource;
        }
        else
        {
          OutputDebugStringA("Failed to create off-screen plain surface from image for SpriteSource: '");
          OutputDebugStringA(filename.c_str());
          OutputDebugStringA("'\n");
        } // END Create...Surface() if

      }
      else
      {
        OutputDebugStringA("Could not get necessary info from image: '");
        OutputDebugStringA(filename.c_str());
        OutputDebugStringA("' for SpriteSource.   (Image may not exist)\n");

      } // End D3DXGetImageInfo...() if
    }
    else
    {
      OutputDebugStringA("Image for SpriteSource with name '");
      OutputDebugStringA(filename.c_str());
      OutputDebugStringA("' has already been loaded.\n");
    } // End "Check for already loaded spritesource" if

  }
  TextureSource* Graphics::GetTextureSource(std::string texture)
  {
    TextureMap::iterator it = TextureSources.find(texture);
    if (it != TextureSources.end())
      return it->second;
    else
      return NULL;
  } // END GetTexture()
  SpriteSource* Graphics::GetSpriteSource(std::string spritesource)
  {
    SurfaceMap::iterator it = SpriteSources.find(spritesource);
    if (it != SpriteSources.end())
      return it->second;
    else
      return NULL;
  } // END GetSpriteSource()
  

  /// *****CLEANUP*****
  void Graphics::ShutDownD3D(void)
  {
    OutputDebugStringA("CLEANING: Direct3D\n");

    if (pSprite) pSprite->Release();
    if (pFont) pFont->Release();
    if (v_buffer) v_buffer->Release();    // close and release the vertex buffer
    if (pShader) pShader->Release();    // close and release the shader effect
    if (pDevice) pDevice->Release();    // close and release the 3D device
    if (pD3D) pD3D->Release();    // close and release Direct3D
  } // END cleanD3D()


  /////////////////////////////////////////////////////////////////////////////////////////////////



  void Graphics::Debug_TrackCamera()
  {
    testDefaultText->SetText("Camera Position: (");

    std::ostringstream convert;   // stream used for the conversion

    convert << CurrentCamera->transform->Position.x;
    testDefaultText->Text += convert.str() + ", ";
    convert.str("");
    convert.clear();
    convert << CurrentCamera->transform->Position.y;
    testDefaultText->Text += convert.str() + ", ";
    convert.str("");
    convert.clear();
    convert << CurrentCamera->transform->Position.z;
    testDefaultText->Text += convert.str() + ")\n";
    convert.str("");
    convert.clear();


    testDefaultText->AppendText("Camera Lookat Point: (");

    convert << CamLookAt.x;
    testDefaultText->Text += convert.str() + ", ";
    convert.str("");
    convert.clear();
    convert << CamLookAt.y;
    testDefaultText->Text += convert.str() + ", ";
    convert.str("");
    convert.clear();
    convert << CamLookAt.z;
    testDefaultText->Text += convert.str() + ")\n";
    convert.str("");
    convert.clear();
  }
}
