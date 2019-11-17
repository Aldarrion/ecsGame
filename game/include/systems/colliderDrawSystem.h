#pragma once

#include "components/components.h"
#include "ecs.h"
#include "math/vec2.h"
#include "math/math.h"

#include "SDL2/SDL.h"

#include <array>
#include <cmath>

namespace eg {
namespace colliderDrawSystem {

//-----------------------------------------------------------------------------
void drawCircle(SDL_Renderer* render, Vec2 pos, float radius) {
    std::array<SDL_Point, 16> points;

    constexpr float anglePart = EG_PI_2 / (points.size() - 1);
    for (int i = 0; i < points.size(); ++i) {
        const float angle = anglePart * i;
        points[i].x = pos.x + std::cos(angle) * radius;
        points[i].y = pos.y + std::sin(angle) * radius;
    }

    if (SDL_RenderDrawLines(render, points.data(), points.size())) {
        std::cerr << "SDL draw lines failded: " << SDL_GetError() << std::endl;
    }
}

//-----------------------------------------------------------------------------
constexpr uint32_t COLLIDER_COLOR = 0xff22ff22;
//-----------------------------------------------------------------------------
enum class Channel {
    RED,
    GREEN,
    BLUE,
    ALPHA
};
//-----------------------------------------------------------------------------
template<Channel channel>
constexpr uint8_t getCol(uint32_t color) {
    if constexpr (channel == Channel::RED) {
        return (color >> 16) & 0xff;
    } else if constexpr (channel == Channel::GREEN) {
        return (color >> 8) & 0xff;
    } else if constexpr (channel == Channel::BLUE) {
        return color && 0xff;
    } else {
        return (color >> 24) & 0xff;
    }
}
//-----------------------------------------------------------------------------
void update() {
    RendererComponent* render = ECS::reg().raw<RendererComponent>();
    uint8_t a = getCol<Channel::ALPHA>(COLLIDER_COLOR);
    SDL_SetRenderDrawColor(render->Renderer, getCol<Channel::RED>(COLLIDER_COLOR), getCol<Channel::GREEN>(COLLIDER_COLOR), getCol<Channel::BLUE>(COLLIDER_COLOR), getCol<Channel::ALPHA>(COLLIDER_COLOR));
    
    ECS::reg().view<const CircleColliderComponent, const PositionComponent>().each([renderer = render->Renderer](auto entity, const CircleColliderComponent& collider, const PositionComponent& pos) {
        drawCircle(renderer, pos.Pos, collider.Radius);
    });
}

} // namespace colliderDrawSystem
} // namespace eg
