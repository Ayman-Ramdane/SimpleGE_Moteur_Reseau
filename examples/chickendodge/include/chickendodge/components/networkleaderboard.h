#pragma once

#include "chickendodge/pch/precomp.h"

#include <chickendodge/components/score.h>
#include <mutex>
#include <thread>

namespace ChickenDodge
{
  class NetworkLeaderboardComponent : public SimpleGE::NetworkComponent
  {
  public:
    static constexpr auto LeaderboardSize = 10;

    static constexpr auto Type = "NetworkLeaderboard";

    NetworkLeaderboardComponent(SimpleGE::Entity& owner) : Component(owner) {}

    void OnMessage(SimpleGE::Network::Connection& connection, const SimpleGE::BaseMessage& msg) override;

  private:
    void SetScore(const std::string& name, int score);
    void Display();

    std::unordered_map<std::string, int> scores;
    std::mutex mutex;
  };
} // namespace ChickenDodge