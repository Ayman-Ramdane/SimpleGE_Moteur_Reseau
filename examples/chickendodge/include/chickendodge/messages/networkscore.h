#pragma once

#include "chickendodge/pch/precomp.h"

namespace ChickenDodge
{
  struct NetworkScore : public SimpleGE::Message<NetworkScore>
  {
    static constexpr MessageType Type = FirstGameType + 3;

    int playerIndex;
    std::vector<std::string> names;
    std::vector<int> scores;

    NetworkScore() = default;
    NetworkScore(int playerIndex, int score, std::initializer_list<std::string> names) : playerIndex(playerIndex), scores(playerIndex), names(names) {}

    void Serialize(SimpleGE::ISerializer& serializer) const override
    {
      BaseMessage::Serialize(serializer);
      serializer.Write(playerIndex);

      serializer.Write(names.size());
      for (const auto& n : names)
      {
        serializer.Write(n);
      }

      serializer.Write(scores.size());
      for (const auto& n : scores)
      {
        serializer.Write(n);
      }
    }

    void Deserialize(SimpleGE::IDeserializer& deserializer) override
    {
      BaseMessage::Deserialize(deserializer);
      deserializer.Read(playerIndex);

      decltype(names)::size_type nbNames{};
      deserializer.Read(nbNames);
      names.resize(nbNames);
      for (int i = 0; i < nbNames; i++)
      {
        deserializer.Read(gsl::at(names, i));
      }

      decltype(scores)::size_type nbScores{};
      deserializer.Read(nbScores);
      names.resize(nbScores);
      for (int i = 0; i < nbScores; i++)
      {
        deserializer.Read(gsl::at(scores, i));
      }
    }
  };
} // namespace ChickenDodge