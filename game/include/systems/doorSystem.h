#pragma once

#include "components/components.h"
#include "ecs.h"

namespace eg {
namespace doorSystem {

//-----------------------------------------------------------------------------
void update() {
    auto pw = ECS::reg().view<Player_tag, PositionComponent>();
    auto& pos = pw.get<PositionComponent>(*pw.begin());

    auto view = ECS::reg().view<DoorComponent, PositionComponent>();

    // If player collides with a door create an entity with a MapLoadComponent with correct info
    view.each([playerPos = pos.Pos](const auto& door, const auto& doorPos) {
        if (playerPos == doorPos.Pos) {
            const auto& currentMap = ECS::reg().get<MapComponent>(*ECS::reg().view<MapComponent>().begin());
            auto [ent, mapInfo] = ECS::reg().create<MapLoadInfo>();
            mapInfo.MapCoords = currentMap.MapCoords + door.Direction;
            mapInfo.PlayerPos = Vec2Int(
                door.Direction.x == 0 ? doorPos.Pos.x : doorPos.Pos.x == 0 ? MAP_WIDTH - 2 : 1,
                door.Direction.y == 0 ? doorPos.Pos.y : doorPos.Pos.y == 0 ? MAP_HEIGHT - 2 : 1
            );
        }
    });
}

} // namespace doorSystem
} // namespace eg