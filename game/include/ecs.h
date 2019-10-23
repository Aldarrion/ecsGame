#pragma once

#include "entt/entity/registry.hpp"
#include "entt/entity/helper.hpp"

namespace eg {
class ECS {
public:
    static ECS& inst();

    static entt::registry& reg();

private:
    entt::registry registry_;
};
}