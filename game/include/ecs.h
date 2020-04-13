#pragma once

#include "entt/entity/registry.hpp"
#include "entt/entity/helper.hpp"

namespace eg {

using Player_tag = entt::tag<"player"_hs>;
using Previous_tag = entt::tag<"previous"_hs>;
using Current_tag = entt::tag<"current"_hs>;
using Impassable_tag = entt::tag<"impassable"_hs>;
using Pushable_tag = entt::tag<"pushable"_hs>;

class ECS {
public:
    static ECS& inst();

    static entt::registry& reg();

    template<typename ComponentT, typename... TagT>
    static auto getSingletonEnt() {
        auto view = ECS::reg().view<ComponentT, TagT...>();
        assert(view.size() == 1);
        return *view.begin();
    }

    template<typename ComponentT, typename... TagT>
    static auto& getSingleton() {
        auto& comp = ECS::reg().get<ComponentT>(getSingletonEnt<ComponentT, TagT...>());
        return comp;
    }

private:
    entt::registry registry_;
};
}