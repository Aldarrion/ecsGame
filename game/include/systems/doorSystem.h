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
    view.each([playerPos = pos.Pos](const DoorComponent& door, const PositionComponent& doorPos) {
        if (playerPos == doorPos.Pos) {
            const auto& currentMap = ECS::reg().get<MapComponent>(*ECS::reg().view<MapComponent>().begin());
            auto [ent, mapInfo] = ECS::reg().create<MapLoadInfo>();
            mapInfo.MapCoords = currentMap.MapCoords + door.Direction;
            
            mapInfo.PlayerPos = Vec2Int(1, 1);
            if (door.Direction.x == 0)
                mapInfo.PlayerPos.x = doorPos.Pos.x / TILE_SIZE;
            else if (door.Direction.x == -1 && int(doorPos.Pos.x / TILE_SIZE) == 0)
                mapInfo.PlayerPos.x = MAP_WIDTH - 2;

            if (door.Direction.y == 0)
                mapInfo.PlayerPos.y = doorPos.Pos.y / TILE_SIZE;
            else if (door.Direction.y == -1 && int(doorPos.Pos.y / TILE_SIZE) == 0)
                mapInfo.PlayerPos.y = MAP_HEIGHT - 2;
        }
    });
}

} // namespace doorSystem
} // namespace eg