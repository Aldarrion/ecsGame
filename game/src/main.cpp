#include "SDL2/SDL.h"

#include "ecs.h"
#include "math/vec2.h"

#include <iostream>

namespace eg {

struct RendererComponent {
    SDL_Renderer* Renderer;
};

struct PositionComponent {
    Vec2 Pos;
};

struct SpriteComponent {
    float Dummy;
};

void initGame() {
    auto [entity, pos, sprite] = ECS::reg().create<PositionComponent, SpriteComponent>();
    pos.Pos = Vec2(100, 50);
}

namespace spriteRenderSystem {
void update() {
    auto rc = ECS::reg().raw<RendererComponent>();

    SDL_RenderClear(rc->Renderer);

    ECS::reg().view<PositionComponent, SpriteComponent>().each([](auto entity, auto& pos, auto& sprite) {
        std::cout << pos.Pos.x() << " " << pos.Pos.y() << std::endl;
    });

    SDL_RenderPresent(rc->Renderer);
}
}

int runGame() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL init failded: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Hello world", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL window creation failded: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    initGame();

    auto rendererEntity = eg::ECS::reg().create();
    ECS::reg().get_or_assign<eg::RendererComponent>(rendererEntity).Renderer = renderer;

    SDL_Event event;
    bool quit = false;

    while (!quit) {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        spriteRenderSystem::update();
    }
    std::cout << "Done" << std::endl;

    return 0;
}

}

int main() {
    int result = eg::runGame();

    return result;
}

