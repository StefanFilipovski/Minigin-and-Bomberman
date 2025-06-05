#pragma once
#include "CollisionResponder.h"

namespace dae {
    class PowerUpComponent;

    class PowerUpCollisionResponder : public CollisionResponder {
    public:
        explicit PowerUpCollisionResponder(PowerUpComponent* powerUp);
        ~PowerUpCollisionResponder() override = default;

        void OnCollide(GameObject* other) override;

    private:
        PowerUpComponent* m_PowerUp;
        bool m_Active{ true };
    };
}