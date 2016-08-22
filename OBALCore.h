///////////////////////////////////////////////////////////////////////////////
//This is the class that creates the OBALCore, which holds all of the systems
//  in the game engine.
//OBALCore BROADCASTS ANY MESSAGES it recieves from WindowsSystem when it 
//  checks and handles messages
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "SystemsBase.h"
#include "MessageRecipientList.h"
#include <vector>
//#include <windows.h>

namespace OBALFramework
{

  class OBALCore
  {
    public:
    OBALCore(const WCHAR* wndtitle, int screenwidth, int screenheight);
    ~OBALCore();
    std::string GetName() { return "OBALCore"; }

    // Initializes all systems in the game.
    void Initialize();
    // Update all the systems until the game is no longer active.
    void GameLoop();

    // Adds a new system to the game.
    void AddSystem(ISystem * system);
    // Destroy all systems in reverse order that they were added.
    void DestroySystems();

    // Broadcasts a message to all systems.
    void BroadcastMessage(Message * message);

    //When systems are added to the OBALCore, they tell the OBALCore
    //to register them to only recieve a certain type of message
    ///void RegisterRecipient(ISystem* system, MGID id);


    private:
      bool _AllSystemsCreated = false;

    // Keep tabs on all systems
    std::vector<ISystem*> _Systems;
    // The last time the game was updated
    unsigned _LastUpTime;
    // Is the game running (true) or being shut down (false)?
    bool _GameActive;

    // Window Settings
    const WCHAR* _windowTitle;
    int _ScreenWidth;
    int _ScreenHeight;

    MessageRecipientList * _Recipients;

    //Graphics * GRAPHICs;
  };


  //A global pointer to the instance of the core
  extern OBALCore* OBAL;
}