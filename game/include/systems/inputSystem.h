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

constexpr float PLAYER_ANIM_TIME = 0.2f;

//-----------------------------------------------------------------------------
void update() {
    auto keyboard = ECS::reg().raw<KeyboardStateComponent>();
    auto pw = ECS::reg().view<Player_tag, PositionComponent>();
    auto& pos = pw.get<PositionComponent>(*pw.begin());
    const auto map = ECS::reg().raw<MapComponent>();

    const Vec2Int intPos(pos.Pos.x / 64, pos.Pos.y / 64);
    Vec2Int posDiff(0, 0);
    if (keyboard->ADown)
        posDiff.x -= 1;
    else if (keyboard->DDown)
        posDiff.x += 1;
    else if (keyboard->WDown)
        posDiff.y -= 1;
    else if (keyboard->SDown)
        posDiff.y += 1;

    const Vec2Int newPos(intPos + posDiff);
    if (!isWallAt(*map, newPos) && isInBounds(newPos)) {
        auto playerAnim = ECS::reg().try_get<PositionAnim>(*ECS::reg().data<Player_tag>());

        // Player does not have animation now
        if (newPos != intPos) {
            const Vec2 newPosFlt(newPos * TILE_SIZE);
            if (!playerAnim) {
                ECS::reg().assign<PositionAnim>(*pw.begin(), pos.Pos, newPosFlt, PLAYER_ANIM_TIME, 0.0f);
            } else {
                const Vec2 reversePos(playerAnim->End + Vec2(posDiff * TILE_SIZE));
                if (playerAnim->Start == reversePos) {
                    // Canceling animation and moving back
                    ECS::reg().replace<PositionAnim>(*pw.begin(), playerAnim->End, playerAnim->Start, PLAYER_ANIM_TIME, 1.0f - playerAnim->CurrentTime);
                }
            }
        }
    }

    *keyboard = {};
}

} // namespace inputSystem
} // namespace eg