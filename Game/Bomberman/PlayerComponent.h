#pragma once
#include "Component.h"
#include "Observer.h"
#include "GameEvents.h"
#include <vec3.hpp>
#include <vector>

namespace dae {
    class SpriteSheetComponent;
    class TransformComponent;

    class PlayerComponent : public Component, public Subject {
    public:
        enum class Direction { Left, Right, Up, Down };

        explicit PlayerComponent(GameObject* owner);
        ~PlayerComponent() override = default;

        void Update(float deltaTime) override;
        int  GetHealth()    const;
        void TakeDamage(int dmg);

        void OnMovementPressed(Direction dir);
        void OnMovementReleased(Direction dir);
        // movement helpers

        void MoveCurrent(float dt);
        void UpdateSpriteState();
        void BeginMove();
        void Move(float dx, float dy);
        void RevertMove();

    private:
         
        // blink/invul
        bool  m_IsInvulnerable{ false };
        float m_InvulTimer{ 0.f }, m_FlashTimer{ 0.f };
        const float m_InvulDuration{ 2.f }, m_FlashInterval{ 0.1f };
        bool  m_SpriteVisible{ true };

        // health
        int   m_health{ 3 };
        bool  m_IsDead{ false };

        // component refs
        SpriteSheetComponent* m_Sprite{ nullptr };
        TransformComponent* m_Transform{ nullptr };

        // collision helper
        glm::vec3 m_lastValidPosition{ 0,0,0 };
        const float s_TileSize = 16.f;

        // movement
        float                     m_speed{ 36.f };
        std::vector<Direction>    m_MovementDirs;

        bool      m_justSpawned{ true };
    };
}
