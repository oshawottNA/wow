///////////////////////////////////////////////////////////////////////////////
//This is the base class for every System that the game engine uses.
//Basically, it virutualizes every common game loop function, such as
//Initialize and Update.
///////////////////////////////////////////////////////////////////////////////

#pragma once

//#include <iostream>
//#include <Vector>
#include "Message.h"
#include <string>

namespace OBALFramework
{
	///System is a pure virtual base class (which is to say, an interface) that is
	///the base class for all systems used by the game. 
	class ISystem
	{
		public:
		// Systems can receive all message send to the Core.  (Borrowed from sample engine)
		virtual void ThrowMessage(Message* message) {};

		// Initialize the system.
		virtual void Initialize() {};

		// All systems are updated every game frame.
		virtual void Update(float timeslice) = 0;

		// All systems provide a string name for debugging.   (Borrowed from sample engine)
		virtual std::string GetName() = 0;

		// All systems need a virtual destructor to have their destructor called 
    virtual ~ISystem() {};
	};
}
