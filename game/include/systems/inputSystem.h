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

#define HS_KEY_CASE(keyCode, var, value) case keyCode: var = value; break;
#define HS_KEY_DOWN_CASE(keyCode, var) HS_KEY_CASE(keyCode, var,  true)
#define HS_KEY_UP_CASE(keyCode, var) HS_KEY_CASE(keyCode, var, false)

//#define HS_IS_KEY_DOWN(keyCode, varName) case keyCode:   return prev.F2Down == false && curr.F2Down == true;

//-----------------------------------------------------------------------------
void keyDown(const SDL_KeyboardEvent& event) {
    auto& k = ECS::getSingleton<KeyboardStateComponent, Current_tag>();
    switch (event.keysym.sym) {
        HS_KEY_DOWN_CASE(SDLK_ESCAPE, k.EscapeDown);
        HS_KEY_DOWN_CASE(SDLK_F2, k.F2Down);
        HS_KEY_DOWN_CASE(SDLK_F9, k.F9Down);
        HS_KEY_DOWN_CASE(SDLK_a, k.ADown);
        HS_KEY_DOWN_CASE(SDLK_s, k.SDown);
        HS_KEY_DOWN_CASE(SDLK_d, k.DDown);
        HS_KEY_DOWN_CASE(SDLK_w, k.WDown);
    }
}

//-----------------------------------------------------------------------------
void keyUp(const SDL_KeyboardEvent& event) {
    auto& k = ECS::getSingleton<KeyboardStateComponent, Current_tag>();
    switch (event.keysym.sym) {
        HS_KEY_UP_CASE(SDLK_ESCAPE, k.EscapeDown);
        HS_KEY_UP_CASE(SDLK_F2, k.F2Down);
        HS_KEY_UP_CASE(SDLK_F9, k.F9Down);
        HS_KEY_UP_CASE(SDLK_a, k.ADown);
        HS_KEY_UP_CASE(SDLK_s, k.SDown);
        HS_KEY_UP_CASE(SDLK_d, k.DDown);
        HS_KEY_UP_CASE(SDLK_w, k.WDown);
    }
}

//-----------------------------------------------------------------------------
constexpr float PLAYER_ANIM_TIME = 0.2f;

//-----------------------------------------------------------------------------
bool isKeyDown(SDL_Keycode key, const KeyboardStateComponent& prev, const KeyboardStateComponent& curr) {
    switch (key) {
        case SDLK_ESCAPE:   return prev.EscapeDown == false && curr.EscapeDown== true;
        case SDLK_F2:       return prev.F2Down == false && curr.F2Down == true;
        case SDLK_F9:       return prev.F9Down == false && curr.F9Down == true;
        case SDLK_a:        return prev.ADown == false && curr.ADown == true;
        case SDLK_s:        return prev.SDown == false && curr.SDown == true;
        case SDLK_d:        return prev.DDown == false && curr.DDown == true;
        case SDLK_w:        return prev.WDown == false && curr.WDown == true;
    }
    return false;
}

//-----------------------------------------------------------------------------
bool isKeyDown(SDL_Keycode key) {
    auto& prev = ECS::getSingleton<KeyboardStateComponent, Previous_tag>();
    auto& curr = ECS::getSingleton<KeyboardStateComponent, Current_tag>();
    return isKeyDown(key, prev, curr);
}

//-----------------------------------------------------------------------------
void systemUpdate() {
    auto& keyboard = ECS::getSingleton<KeyboardStateComponent, Current_tag>();
    auto& prevKeyboard = ECS::getSingleton<KeyboardStateComponent, Previous_tag>();
    auto& config = ECS::getSingleton<ConfigComponent>();

    if (isKeyDown(SDLK_ESCAPE, prevKeyboard, keyboard)) {
        config.ShouldQuit = true;
        return;
    }

    if (isKeyDown(SDLK_F9, prevKeyboard, keyboard)) {
        config.IsSimulationPaused = !config.IsSimulationPaused;
    }
}

//-----------------------------------------------------------------------------
void update(float dTime) {
    auto& keyboard = ECS::getSingleton<KeyboardStateComponent, Current_tag>();
    auto& prevKeyboard = ECS::getSingleton<KeyboardStateComponent, Previous_tag>();
    auto& config = ECS::getSingleton<ConfigComponent>();

    if (isKeyDown(SDLK_F2, prevKeyboard, keyboard)) {
        config.DrawDebugColliders = !config.DrawDebugColliders;
    }

    auto playerEnt = ECS::getSingletonEnt<PositionComponent, Player_tag>();
    auto& pos = ECS::reg().get<PositionComponent>(playerEnt);
    const auto map = ECS::reg().raw<MapComponent>();

    Vec2 posDiff(0, 0);
    if (keyboard.ADown)
        posDiff.x -= 1;
    else if (keyboard.DDown)
        posDiff.x += 1;
    else if (keyboard.WDown)
        posDiff.y -= 1;
    else if (keyboard.SDown)
        posDiff.y += 1;
    
    if (posDiff != Vec2::ZERO()) {
        auto playerAnim = ECS::reg().try_get<PositionAnim>(*ECS::reg().data<Player_tag>());
        if (!playerAnim) {
            const Vec2 newPos(pos.Pos + posDiff * TILE_SIZE);
            if (!isWallAt(*map, newPos) && isInBounds(newPos)) {
                // Player does not move now
                ECS::reg().assign<PositionAnim>(playerEnt, pos.Pos, newPos, PLAYER_ANIM_TIME, 0.0f);
            }
        } else {
            const Vec2 currentDir = (playerAnim->End - playerAnim->Start) / TILE_SIZE;
            if (posDiff == -currentDir) {
                // Canceling animation and moving back
                ECS::reg().replace<PositionAnim>(playerEnt, playerAnim->End, playerAnim->Start, PLAYER_ANIM_TIME, PLAYER_ANIM_TIME - playerAnim->CurrentTime);
            } else if (playerAnim->CurrentTime + dTime > playerAnim->Time) {
                // Anim will end this frame, schedule a new one instead
                const Vec2 newPos(playerAnim->End + posDiff * TILE_SIZE);
                ECS::reg().replace<PositionAnim>(playerEnt, playerAnim->End, newPos, PLAYER_ANIM_TIME, -(playerAnim->CurrentTime + dTime - PLAYER_ANIM_TIME));
            }
        }
    }
}

} // namespace inputSystem
} // namespace eg