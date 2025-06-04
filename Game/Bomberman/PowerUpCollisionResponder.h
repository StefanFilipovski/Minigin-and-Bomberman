#pragma once
#include "CollisionResponder.h"

namespace dae {
    class PowerUpComponent;

    class PowerUpCollisionResponder : public CollisionResponder {
    public:
        explicit PowerUpCollisionResponder(PowerUpComponent* powerUp)
            : m_PowerUp(powerUp) {
        }

        void OnCollide(GameObject* other) override;

    private:
        PowerUpComponent* m_PowerUp;
    };
}