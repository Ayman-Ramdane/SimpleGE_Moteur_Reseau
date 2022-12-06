#include "chickendodge/pch/precomp.h"

#include "chickendodge/components/networkleaderboard.h"

using json = nlohmann::json;

using namespace SimpleGE;

namespace ChickenDodge
{
  void NetworkLeaderboardComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    // TODO: Implémenter le fonctionnement
  }

  void NetworkLeaderboardComponent::SetScore(const std::string& name, int score)
  {
    std::lock_guard<std::mutex> lock(mutex);
    scores[name] = score;
    Display();
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

#ifdef USE_DEBUG_LEADERBOARD
  NetworkLeaderboardComponent::DebugLeaderboardTest::DebugLeaderboardTest(NetworkLeaderboardComponent& owner,
                                                                          std::string_view name, int score,
                                                                          std::chrono::milliseconds freq)
      : owner(owner), name(name), score(score), freq(freq), thread([this]() { Run(); })
  {
  }

  NetworkLeaderboardComponent::DebugLeaderboardTest::~DebugLeaderboardTest()
  {
    done = true;
    thread.join();
  }

  void NetworkLeaderboardComponent::DebugLeaderboardTest::Run()
  {
    done = false;
    while (!done)
    {
      owner.SetScore(name, score);
      score += 250;
      std::this_thread::sleep_for(freq);
    }
  }
#endif // USE_DEBUG_LEADERBOARD
} // namespace ChickenDodge
