#pragma once
#include "CollisionResponder.h"

namespace dae {
    class BombComponent;

    class BombCollisionResponder : public CollisionResponder {
    public:
        explicit BombCollisionResponder(BombComponent* bomb) : m_Bomb(bomb) {}
        void OnCollide(GameObject* other) override;

    private:
        BombComponent* m_Bomb;
    };
}