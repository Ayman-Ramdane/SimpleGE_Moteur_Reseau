#pragma once

#include "chickendodge/pch/precomp.h"

#include "chickendodge/messages/networklogin.h"
#include "chickendodge/messages/networkstart.h"
#include <chickendodge/messages/networkleaderboardmessage.h>
#include <chickendodge/messages/networkscore.h>

namespace ChickenDodge
{
  static inline void RegisterGameMessages()
  {
    using Message = SimpleGE::BaseMessage;

    Message::Register<NetworkLogin>();
    Message::Register<NetworkStart>();
    Message::Register<NetworkScore>();
    Message::Register<NetworkLeaderboardMessage>();
  }
} // namespace ChickenDodge