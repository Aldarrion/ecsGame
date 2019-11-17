#pragma once

#include "components/components.h"
#include "ecs.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

namespace eg {
namespace spriteRenderSystem {

//-----------------------------------------------------------------------------
void update() {
    auto rc = ECS::reg().raw<RendererComponent>();

    SDL_SetRenderDrawColor(rc->Renderer, 128, 128, 128, 255);
    SDL_RenderClear(rc->Renderer);

    ECS::reg().view<const SpriteComponent, const PositionComponent>().each([render = rc->Renderer](auto entity, const SpriteComponent& sprite, const PositionComponent& pos) {
        int w, h;
        SDL_QueryTexture(sprite.Texture, nullptr, nullptr, &w, &h);
        SDL_Rect dst{ int(pos.Pos.x - w / 2.0f), int(pos.Pos.y - h / 2.0f), w, h };
        if (SDL_RenderCopy(render, sprite.Texture, nullptr, &dst) != 0) {
            std::cerr << SDL_GetError() << std::endl;
        }
    });

    SDL_RenderPresent(rc->Renderer);
}

//-----------------------------------------------------------------------------
void cleanup() {
    auto texReg = ECS::reg().raw<TextureRegistry>();
    for (auto tex : texReg->Registry) {
        SDL_DestroyTexture(tex.second);
    }
}

} // namespace spriteRenderSystem
} // namespace eg