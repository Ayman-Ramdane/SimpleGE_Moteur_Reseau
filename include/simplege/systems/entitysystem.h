#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/entity.h>
#include <simplege/systems/system.h>

namespace SimpleGE
{
  class EntitySystem : public ISystem
  {
  public:
    static EntitySystem& Instance()
    {
      static EntitySystem instance;
      return instance;
    }

    static void Remove(const EntityRef& entity)
    {
      entity->SetActive(false);
      Instance().toRemove.push_back(entity);
    }

    EntitySystem(const EntitySystem&) = delete;
    EntitySystem(EntitySystem&&) = delete;
    EntitySystem& operator=(const EntitySystem&) = delete;
    EntitySystem& operator=(EntitySystem&&) = delete;

    void Iterate(const Timing& timing) override { toRemove.clear(); }

  private:
    EntitySystem() = default;
    ~EntitySystem() override = default;

    std::vector<EntityRef> toRemove;
  };
} // namespace SimpleGE