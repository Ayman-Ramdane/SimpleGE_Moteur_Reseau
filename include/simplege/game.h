#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class ISystem;

  class Game
  {
  public:
    enum class RunResult
    {
      Success,
      Failure
    };

    Game() = default;
    Game(const Game&) = delete;
    Game(Game&&) = delete;
    ~Game();
    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&) = delete;

    RunResult Run();

    static void Close() { CloseRequested() = true; }

  protected:
    template<typename T>
    void AddSystem()
    {
      systems.emplace_back(&T::Instance());
    }

    [[nodiscard]] virtual std::string_view GetLaunchScene() const = 0;
    virtual void SetupSystem() = 0;
    virtual void RegisterComponents() const = 0;
    virtual void RegisterMessages() const = 0;

  private:
    static bool& CloseRequested()
    {
      static bool closeRequested{};
      return closeRequested;
    }

    bool LoadScene(std::string_view file);

    std::vector<gsl::not_null<ISystem*>> systems;
  };
} // namespace SimpleGE