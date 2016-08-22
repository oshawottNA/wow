#pragma once

namespace OBALFramework
{
  namespace Mid // Message id
  {
    enum MessageIdType
    {
      None,
      All,
      Quit,
      Collision,
      ToggleDebugInfo,
      CharacterKey,
      HeldKey,
      MouseButton,
      MouseMove,
      FileDrop
    };

  }

  typedef Mid::MessageIdType MSGID;

  /// *****Base Message Class***** ///
  //Base message class. New message types are defined by deriving from this
  //class and mapping defined an element of the MessageIdType enumeration.
  class Message
  {
  public:
    Message(Mid::MessageIdType id) : MessageId(id) {};  // Message object constructor (Initializes MessageId)
    virtual ~Message() {};
    Mid::MessageIdType MessageId;
  };

  //Class RecipientList was here...

  /// *****Message Types(Classes)***** ///
  class Message_None : public Message
  {
  public:
    Message_None() : Message(Mid::None) {};
  };

  class Message_All : public Message
  {
  public:
    Message_All() : Message(Mid::All) {};
  };

  class Message_Quit : public Message
  {
  public:
    Message_Quit() : Message(Mid::Quit) {};
  };

  class Message_Collision : public Message
  {
  public:
    Message_Collision() : Message(Mid::Collision) {};
  };

  class Message_ToggleDebugInfo : public Message
  {
  public:
    Message_ToggleDebugInfo() : Message(Mid::ToggleDebugInfo) {};
  };

  class Message_CharacterKey : public Message
  {
  public:
    Message_CharacterKey() : Message(Mid::CharacterKey) {};
  };

  class Message_HeldKey : public Message
  {
  public:
    Message_HeldKey() : Message(Mid::HeldKey) {};
  };

  class Message_MouseButton : public Message
  {
  public:
    Message_MouseButton() : Message(Mid::MouseButton) {};
  };

  class Message_MouseMove : public Message
  {
  public:
    Message_MouseMove() : Message(Mid::MouseMove) {};
  };

  class Message_FileDrop : public Message
  {
  public:
    Message_FileDrop() : Message(Mid::FileDrop) {};
  };

}
