#pragma once
#include "Component.h"
#include "Observer.h"
#include "GameEvents.h"
#include <vec3.hpp>
#include <vector>
#include <SceneManager.h>
#include "BombComponent.h"

namespace dae {

    class TransformComponent;
    class SpriteSheetComponent;

    class PlayerComponent : public Component, public Subject, public Observer {
    public:
        enum class Direction { Left, Right, Up, Down };

        explicit PlayerComponent(GameObject* owner);
        ~PlayerComponent() override = default;

        // Called every frame
        void Update(float deltaTime) override;

        // Health
        int  GetHealth() const;
        void TakeDamage(int dmg);

        // Movement input (invoked by MoveDirCommand)
        void OnMovementPressed(Direction dir);
        void OnMovementReleased(Direction dir);

        // Bomb placement (invoked by BombCommand)
        void PlaceBomb(Scene& scene);

        // Collision helpers (used internally)
        void BeginMove();
        void Move(float dx, float dy);
        void RevertMove();

        // Observer interface
        void OnNotify(const Event& event) override;

        // Bomb management
        bool CanPlaceBomb() const { return m_ActiveBombCount < m_MaxActiveBombs; }
        void SetMaxBombs(int count) { m_MaxActiveBombs = count; }
        int GetActiveBombCount() const { return m_ActiveBombCount; }

        // Power-up methods
        void EnableDetonator() { m_HasDetonator = true; }
        int GetBombRange() const { return m_BombRange; }
        bool HasDetonator() const { return m_HasDetonator; }

        // Detonator functionality
        void DetonateOldestBomb();

        bool HasMaxBombs() const { return m_MaxActiveBombs >= 4; } // Set a reasonable max
        bool HasMaxRange() const { return m_BombRange >= 2; } // Set a reasonable max

        // Update the increase methods to check limits
        void IncreaseBombCapacity() {
            if (!HasMaxBombs()) {
                m_MaxActiveBombs++;
            }
        }
        void IncreaseBombRange() {
            if (!HasMaxRange()) {
                m_BombRange++;
            }
        }

    private:
        // Core loop
        void MoveCurrent(float dt);
        void UpdateSpriteState();

        // Invulnerability/blink
        bool  m_IsInvulnerable{ false };
        float m_InvulTimer{ 0.f }, m_FlashTimer{ 0.f };
        const float m_InvulDuration{ 2.f }, m_FlashInterval{ 0.1f };
        bool  m_SpriteVisible{ true };

        // Health
        int   m_health{ 3 };
        bool  m_IsDead{ false };

        // Component refs
        TransformComponent* m_Transform{ nullptr };
        SpriteSheetComponent* m_Sprite{ nullptr };

        // Movement state
        float                     m_speed{ 36.f };
        std::vector<Direction>    m_MovementDirs;
        glm::vec3                 m_lastValidPosition{ 0,0,0 };
        bool                      m_justSpawned{ true };
        const float               s_TileSize = 16.f;

        // Bomb management
        int m_MaxActiveBombs{ 1 };
        int m_ActiveBombCount{ 0 };

        // Power-up states
        bool m_HasDetonator{ false };
        int m_BombRange{ 1 };
        std::vector<BombComponent*> m_ActiveBombs; // Track bombs for detonator

        // Footstep timing
        float m_FootstepTimer{ 0.0f };
        float m_FootstepInterval{ 0.5f }; // Adjust this value to change speed
        int m_FootstepChannel{ -1 };

    };
}