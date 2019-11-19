#pragma once

#include "components/components.h"
#include "util/util.h"
#include "ecs.h"

namespace eg {
namespace mapSystem {

//-----------------------------------------------------------------------------
Vec2 coordsToPos(int x, int y) {
    return Vec2(x * TILE_SIZE + TILE_HALF_SIZE, y * TILE_SIZE + TILE_HALF_SIZE);
}

//-----------------------------------------------------------------------------
Vec2 coordsToPos(Vec2Int vec) {
    return coordsToPos(vec.x, vec.y);
}

//-----------------------------------------------------------------------------
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
                    pos.Pos = coordsToPos(x, y);
                    door.Direction = Vec2Int(0, y == 0 ? -1 : 1);
                } else if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) {
                    texturePath = "textures/wall.png";
                    mapComp.Walls.emplace_back(x, y);
                }

                SDL_Texture* texture = loadTexture(texturePath);
                if (!texture)
                    continue;

                auto [ent, pos, sprite, order] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder>();
                pos.Pos = coordsToPos(x, y);
                sprite.Texture = texture;
                order.Order = sortOrder;
            }
        }

        {
            auto [playerEntity, pos, sprite, order, tag, capsule] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder, Player_tag, CapsuleColliderComponent>();
            pos.Pos = coordsToPos(mapLoad.PlayerPos);
            sprite.Texture = loadTexture("textures/player.png");
            SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
            order.Order = 10;
            capsule.Start = Vec2(0, -32);
            capsule.End = Vec2(0, 32);
            capsule.Radius = 16;
        }

        {
            auto [ent, pos, sprite, order, shooter] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder, FlowerShooter>();
            pos.Pos = coordsToPos(3, 6);
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

} // namespace mapSystem
} // namespce eg