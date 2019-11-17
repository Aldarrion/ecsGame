#pragma once

#include "components/components.h"
#include "ecs.h"

#include "SDL2/SDL.h"

namespace eg {

//-----------------------------------------------------------------------------
bool isWallAt(const MapComponent& map, Vec2 pos) {
    Vec2Int coords(pos / TILE_SIZE);
    for (int i = 0; i < map.Walls.size(); ++i) {
        if (map.Walls[i] == coords) {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
bool isInBounds(Vec2 pos) {
    return pos.x >= 0 && pos.y >= 0 && pos.x < MAP_WIDTH * TILE_SIZE && pos.y < MAP_HEIGHT * TILE_SIZE;
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
void keyUp(const SDL_KeyboardEvent& event) {
    auto k = ECS::reg().raw<KeyboardStateComponent>();
    switch (event.keysym.sym) {
        case SDLK_a: k->ADown = false; break;
        case SDLK_s: k->SDown = false; break;
        case SDLK_d: k->DDown = false; break;
        case SDLK_w: k->WDown = false; break;
    }
}

//-----------------------------------------------------------------------------
constexpr float PLAYER_ANIM_TIME = 0.2f;

//-----------------------------------------------------------------------------
void update(float dTime) {
    auto keyboard = ECS::reg().raw<const KeyboardStateComponent>();
    auto pw = ECS::reg().view<Player_tag, PositionComponent>();
    auto& pos = pw.get<PositionComponent>(*pw.begin());
    const auto map = ECS::reg().raw<MapComponent>();

    Vec2 posDiff(0, 0);
    if (keyboard->ADown)
        posDiff.x -= 1;
    else if (keyboard->DDown)
        posDiff.x += 1;
    else if (keyboard->WDown)
        posDiff.y -= 1;
    else if (keyboard->SDown)
        posDiff.y += 1;
    
    if (posDiff != Vec2::ZERO()) {
        auto playerAnim = ECS::reg().try_get<PositionAnim>(*ECS::reg().data<Player_tag>());
        if (!playerAnim) {
            const Vec2 newPos(pos.Pos + posDiff * TILE_SIZE);
            if (!isWallAt(*map, newPos) && isInBounds(newPos)) {
                // Player does not move now
                ECS::reg().assign<PositionAnim>(*pw.begin(), pos.Pos, newPos, PLAYER_ANIM_TIME, 0.0f);
            }
        } else {
            const Vec2 currentDir = (playerAnim->End - playerAnim->Start) / TILE_SIZE;
            if (posDiff == -currentDir) {
                // Canceling animation and moving back
                ECS::reg().replace<PositionAnim>(*pw.begin(), playerAnim->End, playerAnim->Start, PLAYER_ANIM_TIME, PLAYER_ANIM_TIME - playerAnim->CurrentTime);
            } else if (playerAnim->CurrentTime + dTime > playerAnim->Time) {
                // Anim will end this frame, schedule a new one instead
                const Vec2 newPos(playerAnim->End + posDiff * TILE_SIZE);
                ECS::reg().replace<PositionAnim>(*pw.begin(), playerAnim->End, newPos, PLAYER_ANIM_TIME, -(playerAnim->CurrentTime + dTime - PLAYER_ANIM_TIME));
            }
        }
    }
}

} // namespace inputSystem
} // namespace eg