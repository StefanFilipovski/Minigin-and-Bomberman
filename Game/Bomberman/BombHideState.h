#pragma once
#include "IBombState.h"

namespace dae {
    struct BombHideState : IBombState
    {
        void Update(BombComponent& ctx, float dt) override;
    };
}