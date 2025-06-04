#pragma once
#include "Component.h"
#include "PowerUpType.h"
#include "Observer.h"

namespace dae {
    class GameObject;
    class PlayerComponent;

    class PowerUpComponent : public Component, public Subject {
    public:
        PowerUpComponent(GameObject* owner, PowerUpType type);
        ~PowerUpComponent() override = default;

        PowerUpType GetType() const { return m_Type; }
        void OnPickup(PlayerComponent* player);

    private:
        PowerUpType m_Type;
    };
}