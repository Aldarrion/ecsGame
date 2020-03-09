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
    
    ECS::reg().view<const SpriteComponent, const PositionComponent>().each([render = rc->Renderer](auto entity, const SpriteComponent& sprite, const PositionComponent& pos) {
        SDL_Rect dst{ int(pos.Pos.x - sprite.Width / 2.0f), int(pos.Pos.y - sprite.Height / 2.0f), sprite.Width, sprite.Height };
        if (SDL_RenderCopy(render, sprite.Texture, nullptr, &dst) != 0) {
            fprintf(stderr, "%s\n", SDL_GetError());
        }
    });
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