#pragma once

#include "components/components.h"
#include "ecs.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <string>
#include <iostream>

namespace eg {

//-----------------------------------------------------------------------------
void dimFromTex(SpriteComponent& sprite) {
    SDL_QueryTexture(sprite.Texture, nullptr, nullptr, &sprite.Width, &sprite.Height);
}

//-----------------------------------------------------------------------------
SDL_Texture* loadTexture(const char* path) {
    auto texReg = ECS::reg().raw<TextureRegistry>();
    auto found = texReg->Registry.find(path);
    
    if (found != texReg->Registry.end()) {
        return found->second;
    } else {
        SDL_Surface* loadingSurface = IMG_Load(path);
        if (!loadingSurface) {
            fprintf(stderr, "Failed loading texture %s, error: %s\n", path, IMG_GetError());
            return nullptr;
        }

        auto render = ECS::reg().raw<RendererComponent>();
        SDL_Texture* texture = SDL_CreateTextureFromSurface(render->Renderer, loadingSurface);
        SDL_FreeSurface(loadingSurface);

        texReg->Registry.emplace(path, texture);

        return texture;
    }
}

//-----------------------------------------------------------------------------
Vec2 coordsToPos(int x, int y) {
    return Vec2(x * TILE_SIZE + TILE_HALF_SIZE, y * TILE_SIZE + TILE_HALF_SIZE);
}

//-----------------------------------------------------------------------------
Vec2 coordsToPos(Vec2Int vec) {
    return coordsToPos(vec.x, vec.y);
}

//-----------------------------------------------------------------------------
Vec2Int posToCoords(Vec2 pos) {
    Vec2Int coords(pos / TILE_SIZE);
    return coords;
}

} // namespace eg