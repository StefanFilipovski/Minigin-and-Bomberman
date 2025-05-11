#pragma once
#include "IBombState.h"

namespace dae {
    struct BombFuseState : IBombState
    {
        void Update(BombComponent& ctx, float dt) override;
    };
}
