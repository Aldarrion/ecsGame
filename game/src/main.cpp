
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

struct RenderOrder {
    int Order;
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

            auto [ent, pos, sprite, order] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder>();
            pos.Pos = Vec2(x, y);
            sprite.Texture = texture;
            order.Order = 0;
        }
    }

}

void initGame() {
    initMap();

    auto [entity, pos, sprite, order] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder>();
    pos.Pos = Vec2(5, 5);
    sprite.Texture = loadTexture("textures/player.png");
    SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
    order.Order = 10;

    ECS::reg().sort<SpriteComponent>([](const entt::entity lhs, const entt::entity rhs) {
        return ECS::reg().get<RenderOrder>(lhs).Order < ECS::reg().get<RenderOrder>(rhs).Order;
    });
}

namespace spriteRenderSystem {
void update() {
    auto rc = ECS::reg().raw<RendererComponent>();

    SDL_SetRenderDrawColor(rc->Renderer, 128, 128, 128, 255);
    SDL_RenderClear(rc->Renderer);

    ECS::reg().view<SpriteComponent, PositionComponent>().each([render = rc->Renderer](auto entity, auto& sprite, auto& pos) {
        SDL_Rect dst{ int(pos.Pos.x) * 64, int(pos.Pos.y) * 64, 64, 64 };
        if (SDL_RenderCopy(render, sprite.Texture, nullptr, &dst) != 0) {
            std::cerr << SDL_GetError() << std::endl;
        }
    });

    SDL_RenderPresent(rc->Renderer);
}
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

