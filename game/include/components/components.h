#pragma once

#include "math/vec2.h"

#include <vector>
#include <unordered_map>

struct SDL_Texture;
struct SDL_Renderer;

namespace eg {

//-----------------------------------------------------------------------------
struct TextureRegistry {
    std::unordered_map<std::string, SDL_Texture*> Registry;
};

//-----------------------------------------------------------------------------
struct RendererComponent {
    SDL_Renderer* Renderer;
};

//-----------------------------------------------------------------------------
struct RenderOrder {
    int Order;
};

//-----------------------------------------------------------------------------
struct PositionComponent {
    Vec2 Pos;
};

//-----------------------------------------------------------------------------
struct SpriteComponent {
    SDL_Texture* Texture;
};

//-----------------------------------------------------------------------------
struct KeyboardStateComponent {
    bool ADown{ false };
    bool SDown{ false };
    bool DDown{ false };
    bool WDown{ false };
};

//-----------------------------------------------------------------------------
struct DoorComponent {
    Vec2Int Direction;
};

//-----------------------------------------------------------------------------
struct MapComponent {
    Vec2Int MapCoords;
    std::vector<Vec2Int> Walls;
};

//-----------------------------------------------------------------------------
struct MapLoadInfo {
    Vec2Int MapCoords;
    Vec2Int PlayerPos;
};

//-----------------------------------------------------------------------------
struct PositionAnim {
    Vec2 Start;
    Vec2 End;
    float Time;
    float CurrentTime;
};

//-----------------------------------------------------------------------------
struct FlowerShooter {
    static constexpr float TIME_TO_SHOOT{ 1.0f };
    static constexpr Vec2 SHOT_START_POS{ 30.0f, 26.0f };
    //Vec2 Direction;
    float TimeToShoot;
};

}