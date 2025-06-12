#pragma once
#include "Component.h"
#include "Observer.h"
#include "TransformComponent.h"
#include "SpriteSheetComponent.h"
#include "BombComponent.h"
#include <vector>
#include <vec3.hpp>

namespace dae {
    class Scene;

    class Player2BalloonComponent : public Component, public Subject, public Observer {
    public:
        enum class Direction { Left, Right, Up, Down };

        explicit Player2BalloonComponent(GameObject* owner);
        ~Player2BalloonComponent() override = default;

        void Update(float deltaTime) override;

        // Health/Lives system
        int GetLives() const { return m_Lives; }
        void SetLives(int lives) { m_Lives = lives; }
        void TakeDamage(int dmg = 1);
        bool IsDead() const { return m_IsDead; }

        // Movement input
        void OnMovementPressed(Direction dir);
        void OnMovementReleased(Direction dir);

        // Bomb system
        void PlaceBomb(Scene& scene);
        void DetonateOldestBomb();
        bool CanPlaceBomb() const { return m_ActiveBombCount < m_MaxActiveBombs; }

        // Collision helpers
        void BeginMove();
        void Move(float dx, float dy);
        void RevertMove();

        // Observer interface
        void OnNotify(const Event& event) override;

    private:
        void MoveCurrent(float dt);
        void UpdateSpriteState();

        // Core components
        TransformComponent* m_Transform{ nullptr };
        SpriteSheetComponent* m_Sprite{ nullptr };

        // Health/Lives
        int m_Lives{ 3 };
        bool m_IsDead{ false };

        // Movement
        float m_Speed{ 36.f };
        std::vector<Direction> m_MovementDirs;
        glm::vec3 m_LastValidPosition{ 0,0,0 };
        const float m_TileSize = 16.f;

        // Bomb management
        int m_MaxActiveBombs{ 1 };
        int m_ActiveBombCount{ 0 };
        int m_BombRange{ 1 };
        bool m_HasDetonator{ false };
        std::vector<BombComponent*> m_ActiveBombs;
        bool m_NeedsBombCleanup{ false };

        // Invulnerability
        bool m_IsInvulnerable{ false };
        float m_InvulTimer{ 0.f };
        float m_FlashTimer{ 0.f };
        const float m_InvulDuration{ 2.f };
        const float m_FlashInterval{ 0.1f };
        bool m_SpriteVisible{ true };
    };
}