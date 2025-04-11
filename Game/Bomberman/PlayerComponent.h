// PlayerComponent.h
#pragma once
#include "Component.h"
#include <iostream>
#include <vec3.hpp>

namespace dae {

    // A component to manage player-specific behavior.
    class PlayerComponent : public Component {
    public:
        // Constructor takes the owning GameObject.
        explicit PlayerComponent(GameObject* owner);

        // Virtual destructor (defaulted).
        virtual ~PlayerComponent() = default;

        // Called once per frame, if you need to update any state.
        void Update(float deltaTime) override;

        // Methods to handle movement.
        // Call this before applying any movement to save the last valid position.
        void BeginMove();

        // Move the player by the given delta (dx, dy).
        // You could incorporate deltaTime or speed scaling here.
        void Move(float dx, float dy);

        // Revert the move by restoring the last valid position.
        void RevertMove();

        // Health management.
        int GetHealth() const;
        void SetHealth(int health);
        void TakeDamage(int damage);

    private:
        // Stores the last valid position before attempting a move.
        glm::vec3 m_lastValidPosition{ 0.f, 0.f, 0.f };

        // Player health.
        int m_health{ 3 };

        // (Optional) The player's movement speed; you could use this to scale movement.
        float m_speed{ 100.0f };
    };

} // namespace dae
