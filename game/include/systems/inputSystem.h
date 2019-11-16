#pragma once

#include "components/components.h"
#include "ecs.h"

#include "SDL2/SDL.h"

namespace eg {

//-----------------------------------------------------------------------------
bool isWallAt(const MapComponent& map, Vec2Int pos) {
    for (int i = 0; i < map.Walls.size(); ++i) {
        if (map.Walls[i] == pos) {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
bool isInBounds(Vec2Int pos) {
    return pos.x >= 0 && pos.y >= 0 && pos.x < MAP_WIDTH && pos.y < MAP_HEIGHT;
}

namespace inputSystem {

//-----------------------------------------------------------------------------
void keyDown(const SDL_KeyboardEvent& event) {
    auto k = ECS::reg().raw<KeyboardStateComponent>();
    switch (event.keysym.sym) {
        case SDLK_a: k->ADown = true; break;
        case SDLK_s: k->SDown = true; break;
        case SDLK_d: k->DDown = true; break;
        case SDLK_w: k->WDown = true; break;
    }
}

//-----------------------------------------------------------------------------
void update() {
    auto keyboard = ECS::reg().raw<KeyboardStateComponent>();
    auto pw = ECS::reg().view<Player_tag, PositionComponent>();
    auto& pos = pw.get<PositionComponent>(*pw.begin());
    const auto map = ECS::reg().raw<MapComponent>();

    const Vec2Int intPos = Vec2Int(pos.Pos.x / 64, pos.Pos.y / 64);
    Vec2Int newPos = intPos;
    if (keyboard->ADown)
        newPos.x -= 1;
    else if (keyboard->DDown)
        newPos.x += 1;
    else if (keyboard->WDown)
        newPos.y -= 1;
    else if (keyboard->SDown)
        newPos.y += 1;

    if (!isWallAt(*map, newPos) && isInBounds(newPos)) {
        auto playerAnim = ECS::reg().try_get<PositionAnim>(*ECS::reg().data<Player_tag>());

        // Player does not have animation now
        if (newPos != intPos && !playerAnim) {
            ECS::reg().assign<PositionAnim>(*pw.begin(), pos.Pos, Vec2(newPos * TILE_SIZE), 1.0f, 0.0f);
        }
    }

    *keyboard = {};
}

} // namespace inputSystem
} // namespace eg