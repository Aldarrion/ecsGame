
#include "components/components.h"
#include "systems/systems.h"

#include "ecs.h"
#include "math/vec2.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <iostream>


namespace eg {

void initGame() {
    auto [ent, loadInfo] = ECS::reg().create<MapLoadInfo>();
    loadInfo.MapCoords = Vec2Int::ZERO();
    loadInfo.PlayerPos = Vec2Int(5, 5);
}

int runGame() {
    std::cout << "Game started" << std::endl;

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

    int flags = IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags) {
        std::cerr << "SDL IMG init failed, " << IMG_GetError() << std::endl;
        return 1;
    }

    auto rendererEntity = eg::ECS::reg().create();
    ECS::reg().get_or_assign<eg::RendererComponent>(rendererEntity).Renderer = renderer;

    initGame();

    ECS::reg().create<KeyboardStateComponent>();

    SDL_Event event;
    bool quit = false;

    while (!quit) {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    inputSystem::keyDown(event.key);
                    break;
                case SDL_KEYUP:
                    break;
            }
        }

        mapSystem::update();
        inputSystem::update();
        spriteRenderSystem::update();
        doorSystem::update();
    }
    std::cout << "Done" << std::endl;

    return 0;
}

}

int main() {
    int result = eg::runGame();

    return result;
}

