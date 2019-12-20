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
void drawPartCircle(SDL_Renderer* render, Vec2 pos, float radius, float startAngle, float endAngle) {
    std::array<SDL_Point, 16> points;

    const float anglePart = (endAngle - startAngle) / (points.size() - 1);
    for (int i = 0; i < points.size(); ++i) {
        const float angle = startAngle + anglePart * i;
        points[i].x = pos.x + std::cos(angle) * radius;
        points[i].y = pos.y - std::sin(angle) * radius; // -sin because y is down
    }

    if (SDL_RenderDrawLines(render, points.data(), points.size())) {
        fprintf(stderr, "SDL draw lines failed: %s\n", SDL_GetError());
    }
}
//-----------------------------------------------------------------------------
void drawCircle(SDL_Renderer* render, Vec2 pos, float radius) {
    drawPartCircle(render, pos, radius, 0, EG_PI_2);    
}
//-----------------------------------------------------------------------------
void drawLine(SDL_Renderer* render, Vec2 from, Vec2 to) {
    if (SDL_RenderDrawLine(render, from.x, from.y, to.x, to.y)) {
        fprintf(stderr, "SDL draw line failed: %s\n", SDL_GetError());
    }
}
//-----------------------------------------------------------------------------
void drawCapsule(SDL_Renderer* render, Vec2 pos, Vec2 start, Vec2 end, float radius) {
    if (start == end) {
        drawCircle(render, pos + start, radius);
    } else {
        constexpr Vec2 up(0, -1);
        Vec2 v = start - end;
        v.normalize();
        const Vec2 n(-v.y, v.x);

        const float d = up.dot(v);
        const float nd = up.dot(n);

        float alpha = math::lerp(EG_PI, 0, (d + 1) / 2.0f);

        if (nd < 0)
            alpha += EG_PI;

        drawPartCircle(render, pos + start, radius, alpha, alpha + EG_PI);
        drawPartCircle(render, pos + end, radius, alpha + EG_PI, alpha + EG_PI_2);
        drawLine(render, pos + start + n * radius, pos + end + n * radius);
        drawLine(render, pos + start - n * radius, pos + end - n * radius);
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
    auto config = ECS::reg().raw<ConfigComponent>();
    if (!config || !config->DrawDebugColliders) {
        return;
    }

    RendererComponent* render = ECS::reg().raw<RendererComponent>();
    SDL_SetRenderDrawColor(render->Renderer, getCol<Channel::RED>(COLLIDER_COLOR), getCol<Channel::GREEN>(COLLIDER_COLOR), getCol<Channel::BLUE>(COLLIDER_COLOR), getCol<Channel::ALPHA>(COLLIDER_COLOR));

    ECS::reg().view<const CircleColliderComponent, const PositionComponent>().each([renderer = render->Renderer](auto entity, const CircleColliderComponent& circle, const PositionComponent& pos) {
        drawCircle(renderer, pos.Pos, circle.Radius);
    });

    ECS::reg().view<const CapsuleColliderComponent, const PositionComponent>().each([renderer = render->Renderer](auto entity, const CapsuleColliderComponent& capsule, const PositionComponent& pos) {
        drawCapsule(renderer, pos.Pos, capsule.Start, capsule.End, capsule.Radius);
    });
}

} // namespace colliderDrawSystem
} // namespace eg
