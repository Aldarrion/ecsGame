
#include "components/components.h"
#include "systems/doorSystem.h"
#include "systems/flowerShooterSystem.h"
#include "systems/inputSystem.h"
#include "systems/mapSystem.h"
#include "systems/positionAnimationSystem.h"
#include "systems/spriteRenderSystem.h"
#include "systems/colliderDrawSystem.h"
#include "systems/systems.h"

#include "ecs.h"
#include "math/vec2.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <iostream>
#include <chrono>


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

    eg::ECS::reg().create<eg::TextureRegistry>();

    initGame();

    ECS::reg().create<KeyboardStateComponent>();

    SDL_Event event;
    bool quit = false;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    while (true) {
        auto dTime = std::chrono::duration<float>(end - start).count();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                inputSystem::keyDown(event.key);
                break;
            case SDL_KEYUP:
                inputSystem::keyUp(event.key);
                break;
            }
        }

        if (quit)
            break;

        mapSystem::update();
        inputSystem::update(dTime);

        flowerShooterSystem::update(dTime);
        positionAnimationSystem::update(dTime);

        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_RenderClear(renderer);
            spriteRenderSystem::update();
            colliderDrawSystem::update();
        SDL_RenderPresent(renderer);

        doorSystem::update();

        start = end;
        end = std::chrono::high_resolution_clock::now();
    }

    spriteRenderSystem::cleanup();

    std::cout << "Done" << std::endl;

    return 0;
}

}

int main(int argc, char* args[]) {
    int result = eg::runGame();

    return result;
}

