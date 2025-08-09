#pragma once
#include "CollisionResponder.h"

namespace dae {
    class LadderCollisionResponder : public CollisionResponder {
    public:
        void OnCollide(GameObject* other) override {
            // Ladders are static, they don't respond to collision
            // The player checks for ladder collision
            (void)other;
        }
    };
}