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
void createTile(int x, int y, const char* texturePath, int sortOrder) {
    SDL_Texture* texture = loadTexture(texturePath);
    if (!texture)
        return;

    auto [ent, pos, sprite, order] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder>();
    pos.Pos = coordsToPos(x, y);
    sprite.Texture = texture;
    dimFromTex(sprite);
    SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
    order.Order = sortOrder;
}

//-----------------------------------------------------------------------------
// " ← → ← ↓"
const char16_t* mapDefs[][9] = {
    { // Start
        u"xxxxxxxdxxxx",
        u"x..........x",
        u"x...→......x",
        u"x..........x",
        u"d..........x",
        u"x..........x",
        u"x..........x",
        u"x..........x",
        u"xxxxdxxxxxxx",
    },
    { // L1
        u"xxdxxxxxxxxx",
        u"x..........x",
        u"x..........x",
        u"x.......x..x",
        u"x..........x",
        u"x..........x",
        u"x..........x",
        u"x..........x",
        u"xxxxxxxdxxxx",
    },
    { // L2
        u"xxxxxxxxxxxx",
        u"x..........x",
        u"x..→.......x",
        u"x.......x..x",
        u"x.......x..d",
        u"x..........x",
        u"x..........x",
        u"x..........x",
        u"xxxxxxxxxxxx",
    },
    { // L3
        u"xxxxdxxxxxxx",
        u"x..........x",
        u"x..→.......x",
        u"x.......x..x",
        u"x.......x..x",
        u"x.......x..x",
        u"x..........x",
        u"x..........x",
        u"xxxxxxxxxxxx",
    },
    { // Witch
        u"xxxxxxxxxxxx",
        u"x..........x",
        u"x..→.......x",
        u"x.......x..x",
        u"x.......x..x",
        u"x.......x..x",
        u"x.......x..x",
        u"x..........x",
        u"xxdxxxxxxxxx",
    }
};

//-----------------------------------------------------------------------------
int coordsToMap(Vec2Int coords) {
    if (coords == Vec2Int(0, 0)) {
        return 0;
    } else if (coords == Vec2Int(-1, 0)) {
        return 2;
    } else if (coords == Vec2Int(0, -1)) {
        return 1;
    } else if (coords == Vec2Int(0, 1)) {
        return 3;
    } else if (coords == Vec2Int(0, -2)) {
        return 4;
    } else {
        assert(!"Invalid coords to map passed");
        return 0;
    }
}


//-----------------------------------------------------------------------------
void update() {
    // Process all MapLoadComponents and load specified map based on the data present
    ECS::reg().view<MapLoadInfo>().each([](const MapLoadInfo& mapLoad){
        
        // Cleanup
        auto maps = ECS::reg().view<MapComponent>();
        ECS::reg().destroy(maps.begin(), maps.end());
        
        auto doors = ECS::reg().view<DoorComponent>();
        ECS::reg().destroy(doors.begin(), doors.end());

        auto sprites = ECS::reg().view<SpriteComponent>();
        ECS::reg().destroy(sprites.begin(), sprites.end());

        auto players = ECS::reg().view<Player_tag>();
        ECS::reg().destroy(players.begin(), players.end());

        auto [mapEnt, mapComp] = ECS::reg().create<MapComponent>();
        mapComp.MapCoords = mapLoad.MapCoords;

        // Load the map
        auto mapIdx = coordsToMap(mapLoad.MapCoords);
        const char16_t** map = mapDefs[mapIdx];
        
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            assert(std::char_traits<char16_t>::length(map[y]));
            for (int x = 0; x < MAP_WIDTH; ++x) {
                {
                    int sortOrder = 0;
                    const char* texturePath = "textures/grass_tile.png";
                    createTile(x, y, texturePath, sortOrder);
                }

                switch (map[y][x]) {
                    case u'x':
                    {
                        const char* texturePath = "textures/tree.png";
                        mapComp.ImpassableTiles.emplace_back(x, y);
                        createTile(x, y, texturePath, 1);
                        break;
                    }
                    case u'→':
                    {
                        auto [ent, pos, sprite, order, shooter] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder, FlowerShooter>();
                        pos.Pos = coordsToPos(x, y);
                        sprite.Texture = loadTexture("textures/flowerShooter.png");
                        dimFromTex(sprite);
                        SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
                        order.Order = 5;
                        shooter.TimeToShoot = FlowerShooter::TIME_TO_SHOOT;

                        break;
                    }
                    case u'd':
                    {
                        const char* texturePath = "textures/door.png";
                        createTile(x, y, texturePath, 1);

                        auto [ent, pos, door] = ECS::reg().create<PositionComponent, DoorComponent>();
                        pos.Pos = coordsToPos(x, y);

                        door.Direction = Vec2Int::ZERO();
                        
                        if (x == 0)
                            door.Direction.x = -1;
                        else if (x == MAP_WIDTH - 1)
                            door.Direction.x = 1;
                        
                        if (y == 0)
                            door.Direction.y = -1;
                        else if (y == MAP_HEIGHT - 1)
                            door.Direction.y = 1;


                        break;
                    }
                }
            }
        }

        // Create player
        {
            auto [playerEntity, pos, sprite, order, tag, capsule] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder, Player_tag, CapsuleColliderComponent>();
            pos.Pos = coordsToPos(mapLoad.PlayerPos);
            sprite.Texture = loadTexture("textures/player.png");
            dimFromTex(sprite);
            SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
            order.Order = 10;
            capsule.Start = Vec2(4, -8);
            capsule.End = Vec2(4, 14);
            capsule.Radius = 12;
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