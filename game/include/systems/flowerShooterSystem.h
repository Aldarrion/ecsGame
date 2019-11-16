#pragma once

#include "components/components.h"
#include "util/util.h"

#include "ecs.h"

namespace eg {
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