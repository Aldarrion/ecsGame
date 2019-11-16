#pragma once

#include "components/components.h"
#include "ecs.h"

namespace eg {
namespace positionAnimationSystem {

//-----------------------------------------------------------------------------
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

} // namespace positionAnimationSystem
} // namespace eg