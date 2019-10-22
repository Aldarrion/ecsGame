
#include "ecs.h"
#include "math/vec2.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <iostream>
#include <string>

namespace eg {

struct RendererComponent {
    SDL_Renderer* Renderer;
};

struct PositionComponent {
    Vec2 Pos;
};

struct SpriteComponent {
    SDL_Texture* Texture;
};

SDL_Texture* loadTexture(const std::string& path) {
    // TODO(pavel): Maintain a texture registry - do not load duplicate textures
    SDL_Surface* loadingSurface = IMG_Load(path.c_str());
    if (!loadingSurface) {
        std::cerr << "Failed loading texture " << path << ", error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    auto render = ECS::reg().raw<RendererComponent>();
    SDL_Texture* texture = SDL_CreateTextureFromSurface(render->Renderer, loadingSurface);
    SDL_FreeSurface(loadingSurface);
    
    return texture;
}

void initMap() {
    constexpr int WIDTH = 12;
    constexpr int HEIGHT = 9;
    constexpr int TILE_SIZE = 64;

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            std::string texturePath = "textures/floor.png";
            if (x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT - 1)
                texturePath = "textures/wall.png";

            SDL_Texture* texture = loadTexture(texturePath);
            if (!texture)
                continue;

            auto [ent, pos, sprite] = ECS::reg().create<PositionComponent, SpriteComponent>();
            pos.Pos = Vec2(x * TILE_SIZE, y * TILE_SIZE);
            sprite.Texture = texture;
        }
    }

}

void initGame() {
    initMap();

    auto [entity, pos, sprite] = ECS::reg().create<PositionComponent, SpriteComponent>();
    pos.Pos = Vec2(100, 50);
}

namespace spriteRenderSystem {
void update() {
    auto rc = ECS::reg().raw<RendererComponent>();

    SDL_RenderClear(rc->Renderer);

    ECS::reg().view<PositionComponent, SpriteComponent>().each([render = rc->Renderer](auto entity, auto& pos, auto& sprite) {
        SDL_Rect dst{ int(pos.Pos.x()), int(pos.Pos.y()), 64, 64 };
        SDL_RenderCopy(render, sprite.Texture, nullptr, &dst);
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

    int flags = IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags) {
        std::cerr << "SDL IMG init failed, " << IMG_GetError() << std::endl;
        return 1;
    }

    auto rendererEntity = eg::ECS::reg().create();
    ECS::reg().get_or_assign<eg::RendererComponent>(rendererEntity).Renderer = renderer;

    initGame();

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

