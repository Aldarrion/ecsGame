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
    Vec2Int Pos;
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

}