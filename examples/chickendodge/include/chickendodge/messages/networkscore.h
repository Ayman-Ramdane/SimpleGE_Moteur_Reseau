#pragma once

#include "chickendodge/pch/precomp.h"
#include <string_view>

namespace ChickenDodge
{
  struct NetworkScore : public SimpleGE::Message<NetworkScore>
  {
    static constexpr MessageType Type = FirstGameType + 3;

    int playerScore;
    std::string playerName;

    NetworkScore() = default;
    NetworkScore(int playerScore, std::string playerName) : playerScore(playerScore), playerName(std::move(playerName))
    {
    }

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      serializer.Write(playerScore);
      serializer.Write(playerName);
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      deserializer.Read(playerScore);
      deserializer.Read(playerName);
    };
  };
} // namespace ChickenDodge