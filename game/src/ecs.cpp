#include "ecs.h"

namespace eg {

//-----------------------------------------------------------------------------
ECS& ECS::inst() {
    static ECS instance;
    return instance;
}

//-----------------------------------------------------------------------------
entt::registry& ECS::reg() {
    return ECS::inst().registry_;
}

}