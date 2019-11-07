#pragma once

#include "components/components.h"

#include "ecs.h"

#include "math/vec2.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <string>
#include <iostream>

namespace eg {

static constexpr int MAP_WIDTH = 12;
static constexpr int MAP_HEIGHT = 9;
static constexpr int TILE_SIZE = 64;

//-----------------------------------------------------------------------------
bool isWallAt(const MapComponent& map, Vec2Int pos) {
    for (int i = 0; i < map.Walls.size(); ++i) {
        if (map.Walls[i] == pos) {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
bool isInBounds(Vec2Int pos) {
    return pos.x >= 0 && pos.y >= 0 && pos.x < MAP_WIDTH && pos.y < MAP_HEIGHT;
}

//-----------------------------------------------------------------------------
SDL_Texture* loadTexture(const std::string& path) {
    auto texReg = ECS::reg().raw<TextureRegistry>();
    auto found = texReg->Registry.find(path);
    
    if (found != texReg->Registry.end()) {
        return found->second;
    } else {
        SDL_Surface* loadingSurface = IMG_Load(path.c_str());
        if (!loadingSurface) {
            std::cerr << "Failed loading texture " << path << ", error: " << IMG_GetError() << std::endl;
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
namespace spriteRenderSystem {
void update() {
    auto rc = ECS::reg().raw<RendererComponent>();

    SDL_SetRenderDrawColor(rc->Renderer, 128, 128, 128, 255);
    SDL_RenderClear(rc->Renderer);

    ECS::reg().view<const SpriteComponent, const PositionComponent>().each([render = rc->Renderer](auto entity, const SpriteComponent& sprite, const PositionComponent& pos) {
        int w, h;
        SDL_QueryTexture(sprite.Texture, nullptr, nullptr, &w, &h);
        SDL_Rect dst{ int(pos.Pos.x), int(pos.Pos.y), w, h };
        if (SDL_RenderCopy(render, sprite.Texture, nullptr, &dst) != 0) {
            std::cerr << SDL_GetError() << std::endl;
        }
    });

    SDL_RenderPresent(rc->Renderer);
}

void cleanup() {
    auto texReg = ECS::reg().raw<TextureRegistry>();
    for (auto tex : texReg->Registry) {
        SDL_DestroyTexture(tex.second);
    }
}
}

//-----------------------------------------------------------------------------
namespace inputSystem {
void keyDown(const SDL_KeyboardEvent& event) {
    auto k = ECS::reg().raw<KeyboardStateComponent>();
    switch (event.keysym.sym) {
        case SDLK_a: k->ADown = true; break;
        case SDLK_s: k->SDown = true; break;
        case SDLK_d: k->DDown = true; break;
        case SDLK_w: k->WDown = true; break;
    }
}

//-----------------------------------------------------------------------------
void update() {
    auto keyboard = ECS::reg().raw<KeyboardStateComponent>();
    auto pw = ECS::reg().view<Player_tag, PositionComponent>();
    auto& pos = pw.get<PositionComponent>(*pw.begin());
    const auto map = ECS::reg().raw<MapComponent>();

    const Vec2Int intPos = Vec2Int(pos.Pos.x / 64, pos.Pos.y / 64);
    Vec2Int newPos = intPos;
    if (keyboard->ADown)
        newPos.x -= 1;
    else if (keyboard->DDown)
        newPos.x += 1;
    else if (keyboard->WDown)
        newPos.y -= 1;
    else if (keyboard->SDown)
        newPos.y += 1;

    if (!isWallAt(*map, newPos) && isInBounds(newPos)) {
        auto playerAnimW = ECS::reg().view<Player_tag, PositionAnim>();
        // Player does not have animation now
        if (newPos != intPos && playerAnimW.empty()) {
            ECS::reg().assign<PositionAnim>(*pw.begin(), pos.Pos, Vec2(newPos.x * TILE_SIZE, newPos.y * TILE_SIZE), 1.0f, 0.0f);
        }
    }

    *keyboard = {};
}
}

//-----------------------------------------------------------------------------
namespace positionAnimationSystem {
void update(float dTime) {
    ECS::reg().view<PositionComponent, PositionAnim>().each([dTime](auto ent, auto& pos, auto& anim) {
        anim.CurrentTime += dTime;
 
        if (anim.CurrentTime >= anim.Time) {
            pos.Pos = anim.End;
            ECS::reg().remove<PositionAnim>(ent);
        } else {
            const float t = (anim.CurrentTime / anim.Time);
            pos.Pos = (1 - t) * anim.Start + t * anim.End;
        }
    });
}
}

//-----------------------------------------------------------------------------
namespace doorSystem {
void update() {
    auto pw = ECS::reg().view<Player_tag, PositionComponent>();
    auto& pos = pw.get<PositionComponent>(*pw.begin());

    auto view = ECS::reg().view<DoorComponent, PositionComponent>();

    // If player collides with a door create an entity with a MapLoadComponent with correct info
    view.each([playerPos = pos.Pos](const auto& door, const auto& doorPos) {
        if (playerPos == doorPos.Pos) {
            const auto& currentMap = ECS::reg().get<MapComponent>(*ECS::reg().view<MapComponent>().begin());
            auto [ent, mapInfo] = ECS::reg().create<MapLoadInfo>();
            mapInfo.MapCoords = currentMap.MapCoords + door.Direction;
            mapInfo.PlayerPos = Vec2Int(
                door.Direction.x == 0 ? doorPos.Pos.x : doorPos.Pos.x == 0 ? MAP_WIDTH - 2 : 1,
                door.Direction.y == 0 ? doorPos.Pos.y : doorPos.Pos.y == 0 ? MAP_HEIGHT - 2 : 1
            );
        }
    });
}
}

//-----------------------------------------------------------------------------
namespace mapSystem {
void update() {
    // Process all MapLoadComponents and load specified map based on the data present
    ECS::reg().view<MapLoadInfo>().each([](const auto& mapLoad){
        auto maps = ECS::reg().view<MapComponent>();
        ECS::reg().destroy(maps.begin(), maps.end());
        
        auto doors = ECS::reg().view<DoorComponent>();
        ECS::reg().destroy(doors.begin(), doors.end());

        auto sprites = ECS::reg().view<SpriteComponent>();
        ECS::reg().destroy(sprites.begin(), sprites.end());

        auto players = ECS::reg().view<Player_tag>();
        ECS::reg().destroy(players.begin(), players.end());
        
        auto [mapEnt, mapComp] = ECS::reg().create<MapComponent>();

        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                int sortOrder = 0;
                std::string texturePath = "textures/floor.png";
                if (x == 5 && (y == 0 || y == MAP_HEIGHT - 1)) {
                    texturePath = "textures/door.png";
                    auto [ent, pos, door] = ECS::reg().create<PositionComponent, DoorComponent>();
                    pos.Pos = Vec2(x * 64, y * 64);
                    door.Direction = Vec2Int(0, y == 0 ? -1 : 1);
                } else if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) {
                    texturePath = "textures/wall.png";
                    mapComp.Walls.emplace_back(x, y);
                }

                SDL_Texture* texture = loadTexture(texturePath);
                if (!texture)
                    continue;

                auto [ent, pos, sprite, order] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder>();
                pos.Pos = Vec2(x * 64, y * 64);
                sprite.Texture = texture;
                order.Order = sortOrder;
            }
        }

        {
            auto [playerEntity, pos, sprite, order, tag] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder, Player_tag>();
            pos.Pos = Vec2(mapLoad.PlayerPos) * 64;
            sprite.Texture = loadTexture("textures/player.png");
            SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
            order.Order = 10;
        }

        {
            auto [ent, pos, sprite, order, shooter] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder, FlowerShooter>();
            pos.Pos = Vec2(3 * 64, 6 * 64);
            sprite.Texture = loadTexture("textures/flowerShooter.png");
            SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
            order.Order = 5;
            shooter.TimeToShoot = FlowerShooter::TIME_TO_SHOOT;
        }
    });

    auto loadedView = ECS::reg().view<MapLoadInfo>();
    ECS::reg().destroy(loadedView.begin(), loadedView.end());

    ECS::reg().sort<SpriteComponent>([](const entt::entity lhs, const entt::entity rhs) {
        return ECS::reg().get<RenderOrder>(lhs).Order < ECS::reg().get<RenderOrder>(rhs).Order;
    });
}
}

namespace flowerShooterSystem {
void update(float dTime) {
    ECS::reg().view<const PositionComponent, FlowerShooter>().each([dTime](const PositionComponent& flowerPos, FlowerShooter& shooter) {
        shooter.TimeToShoot -= dTime;
        if (shooter.TimeToShoot <= 0) {
            shooter.TimeToShoot = FlowerShooter::TIME_TO_SHOOT;
            
            auto [ent, pos, sprite, order, anim] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder, PositionAnim>();
            pos.Pos = flowerPos.Pos + FlowerShooter::SHOT_START_POS;
            sprite.Texture = loadTexture("textures/flowerProjectile.png");
            SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
            order.Order = 6;
            anim.CurrentTime = 0;
            anim.Time = 5;
            anim.Start = pos.Pos;
            anim.End = pos.Pos + Vec2(TILE_SIZE * 10, 0);
        }
    });
}
} // namespace flowerShooterSystem

} // namespace eg