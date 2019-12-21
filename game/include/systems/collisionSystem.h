#pragma once

#include "components/components.h"
#include "ecs.h"

namespace eg {
namespace collisionSystem {

//-----------------------------------------------------------------------------
bool isColliding(Vec2 circlePos, const CircleColliderComponent& circle, Vec2 capsulePos, const CapsuleColliderComponent& capsule) {
    // Find nearest point on line segment to circle center
    
    // If neareset point distance to circle center is < circle.Radius + capsule.Radius -> return true

    return false;
}

//-----------------------------------------------------------------------------
void update() {

}

} // namespace collisionSystem
} // namespace eg