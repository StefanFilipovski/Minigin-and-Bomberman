#pragma once
#include "CollisionResponder.h"

namespace dae {
    class PlatformCollisionResponder : public CollisionResponder {
    public:
        void OnCollide(GameObject* other) override {
            // Platforms are static, they don't respond to collision
            // The player checks for platform collision
            (void)other;
        }
    };
}