#pragma once

namespace dae {
    class BombComponent;

    struct IBombState {
        virtual ~IBombState() = default;
        virtual void Update(BombComponent& ctx, float dt) = 0;
    };

    //to friend 
    struct BombFuseState;
    struct BombHideState;
}
