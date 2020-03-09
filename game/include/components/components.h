#pragma once

#include "math/vec2.h"

#include <vector>
#include <unordered_map>

struct SDL_Texture;
struct SDL_Renderer;

namespace eg {

static constexpr int MAP_WIDTH = 12;
static constexpr int MAP_HEIGHT = 9;
static constexpr int TILE_SIZE = 128;
static constexpr int TILE_HALF_SIZE = TILE_SIZE / 2;

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
    int Width{ 0 };
    int Height{ 0 };
    SDL_Texture* Texture;
};

//-----------------------------------------------------------------------------
struct KeyboardStateComponent {
    bool EscapeDown{ false };
    bool F2Down{ false };
    bool F9Down{ false };
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
    std::vector<Vec2Int> ImpassableTiles;
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
    static constexpr Vec2 SHOT_START_POS{ 28 - 42 / 2.0f, 9 - 80 / 2.0f }; // 28, 9  -  42, 80
    //Vec2 Direction;
    float TimeToShoot;
};

//-----------------------------------------------------------------------------
struct CircleColliderComponent {
    Vec2 Position{ 0.0f };
    float Radius;
};

//-----------------------------------------------------------------------------
struct CapsuleColliderComponent {
    Vec2 Start;
    Vec2 End;
    float Radius;
};

//-----------------------------------------------------------------------------
struct ConfigComponent {
    bool ShouldQuit{ false };
    bool IsSimulationPaused{ false };
    bool DrawDebugColliders{ false };
};

}