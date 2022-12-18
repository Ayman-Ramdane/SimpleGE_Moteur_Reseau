#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/networkleaderboard.h"
#include <chickendodge/messages/networkleaderboardmessage.h>
#include <string>
#include <utility>
#include <vector>

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  void NetworkLeaderboardComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    if (!msg.Is<NetworkLeaderboardMessage>())
    {
      return;
    }

    const auto& scoreMsg = msg.Get<NetworkLeaderboardMessage>();

    std::vector<std::pair<std::string, int>> leaderboard = scoreMsg.leaderBoard;

    for (auto& pair : leaderboard)
    {
      SetScore(pair.first, pair.second);
    }
    Display();
  }

  void NetworkLeaderboardComponent::SetScore(const std::string& name, int score)
  {
    std::lock_guard<std::mutex> lock(mutex);
    scores[name] = score;
  }

  void NetworkLeaderboardComponent::Display()
  {
    std::vector<std::pair<std::string, int>> sorted(scores.begin(), scores.end());
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
    fmt::print("{0:=^{1}}\n", " Leaderboard ", 30);
    for (int i = 0; i < LeaderboardSize && i < sorted.size(); i++)
    {
      fmt::print("  {:<15}: {:>5}\n", sorted[i].first, sorted[i].second);
    }
    fmt::print("{0:=^{1}}\n\n", "", 30);
  }
} // namespace ChickenDodge
