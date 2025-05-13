#pragma once
#include "Component.h"
#include "Observer.h"
#include "GameEvents.h"
#include <vec3.hpp>
#include <vector>
#include <SceneManager.h>

namespace dae {

    class TransformComponent;
    class SpriteSheetComponent;

    class PlayerComponent : public Component, public Subject {
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
    };
}
