#pragma once

#include "chickendodge/pch/precomp.h"
#include <utility>
#include <vector>

namespace ChickenDodge
{
  struct NetworkLeaderboardMessage : public SimpleGE::Message<NetworkLeaderboardMessage>
  {
    static constexpr MessageType Type = FirstGameType + 2;

    std::vector<std::pair<std::string, int>> leaderBoard;

    NetworkLeaderboardMessage() = default;
    NetworkLeaderboardMessage(std::vector<std::pair<std::string, int>> leaderBoard)
        : leaderBoard(std::move(leaderBoard))
    {
    }

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);

      serializer.Write(leaderBoard.size());

      for (const auto& n : leaderBoard)
      {
        serializer.Write(n.first);
        serializer.Write(n.second);
      }
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);

      decltype(leaderBoard)::size_type nbBoard{};
      deserializer.Read(nbBoard);
      leaderBoard.resize(nbBoard);
      for (int i = 0; i < nbBoard; i++)
      {
        deserializer.Read(leaderBoard[i].first);
        deserializer.Read(leaderBoard[i].second);
      }
    }
  };
} // namespace ChickenDodge