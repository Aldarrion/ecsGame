#pragma once

#include "components/components.h"
#include "util/util.h"
#include "ecs.h"

namespace eg {
namespace mapSystem {

//-----------------------------------------------------------------------------
enum class TileType {
    None,
    Impassable,
    Pushable,
};

//-----------------------------------------------------------------------------
void createTile(int x, int y, const char* texturePath, int sortOrder, TileType type) {
    SDL_Texture* texture = loadTexture(texturePath);
    if (!texture)
        return;

    auto [ent, pos, sprite, order] = ECS::reg().create<PositionComponent, SpriteComponent, RenderOrder>();

    if (type == TileType::Impassable)
        ECS::reg().assign<Impassable_tag>(ent);
    else if (type == TileType::Pushable)
        ECS::reg().assign<Pushable_tag>(ent);

    pos.Pos = coordsToPos(x, y);
    sprite.Texture = texture;
    dimFromTex(sprite);
    SDL_SetTextureBlendMode(sprite.Texture, SDL_BLENDMODE_BLEND);
    order.Order = sortOrder;
}

//-----------------------------------------------------------------------------
void createRoad(int x, int y, const char* texturePath) {
    createTile(x, y, texturePath, 1, TileType::None);
    
    // Roads at corners are not allowed
    assert(x != 0 || y != 0);
    assert(x != MAP_WIDTH - 1 || y != MAP_HEIGHT - 1);
    
    if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
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

        assert(door.Direction != Vec2Int::ZERO());
    }
}

//-----------------------------------------------------------------------------
//  ← → ← ↓ 
//  ┌ ┐ └ ┘ ┤ ─ │ 
const char16_t* mapDefs[][9] = {
    { // Start
        u"xxxxxxx│xxxx",
        u"x......│...x",
        u"x...→..└┐..x",
        u"x......┌┘..x",
        u"───────┤...x",
        u"x..#...│...x",
        u"x...┌──┘...x",
        u"x...│......x",
        u"xxxx│xxxxxxx",
    },
    { // L1
        u"xxx│xxxxxxxx",
        u"x..└┐......x",
        u"x...│......x",
        u"x...└──┐x..x",
        u"x.....x└┐..x",
        u"x.......│..x",
        u"x......x│..x",
        u"x......┌┘..x",
        u"xxxxxxx│xxxx",
    },
    { // L2
        u"xxxxxxxxxxxx",
        u"x..........x",
        u"x..→.......x",
        u"x.......x..x",
        u"x.......x..x",
        u"x..........x",
        u"x.........#─",
        u"x..........x",
        u"xxxxxxxxxxxx",
    },
    { // L3
        u"xxxx│xxxxxxx",
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
        u"x..........x",
        u"x.......x..x",
        u"x.......x..x",
        u"x.......x..x",
        u"x.......x..x",
        u"x..........x",
        u"xxx│xxxxxxxx",
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
                    createTile(x, y, texturePath, sortOrder, TileType::None);
                }

                auto mapChar = map[y][x];
                switch (mapChar) {
                    case u'x':
                    {
                        const char* texturePath = "textures/tree.png";
                        createTile(x, y, texturePath, 1, TileType::Impassable);
                        break;
                    }
                    case u'│':
                    {
                        createRoad(x, y, "textures/roadVertical.png");
                        break;
                    }
                    case u'─':
                    {
                        createRoad(x, y, "textures/roadHorizontal.png");
                        break;
                    }
                    case u'┌':
                    {
                        createRoad(x, y, "textures/roadTopLeft.png");
                        break;
                    }
                    case u'┐':
                    {
                        createRoad(x, y, "textures/roadTopRight.png");
                        break;
                    }
                    case u'└':
                    {
                        createRoad(x, y, "textures/roadBottomLeft.png");
                        break;
                    }
                    case u'┘':
                    {
                        createRoad(x, y, "textures/roadBottomRight.png");
                        break;
                    }
                    case u'┤':
                    {
                        createRoad(x, y, "textures/roadVerticalLeft.png");
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
                    case u'#':
                    {
                        const char* texturePath = "textures/box.png";
                        createTile(x, y, texturePath, 6, TileType::Pushable);
                        break;
                    }
                    case u'.':
                        break; // Grass is already handled above - we put it on each tile now
                    default:
                    {
                        assert(!"Invalid map character");
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