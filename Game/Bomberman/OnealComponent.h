#pragma once

#include "BaseEnemyComponent.h"

namespace dae {
    class GameObject;

    class OnealComponent : public BaseEnemyComponent {
    public:
        OnealComponent(GameObject* owner,
            float speed,
            float moveInterval,
            float chaseRange,
            std::vector<std::vector<bool>> grid,
            glm::ivec2 gridSize,
            float tileSize,
            float gridOffsetY);

        ~OnealComponent() override = default;

    protected:
        void UpdateAI(float dt) override;
        void InitializeAnimation() override;
        void PlayDeathAnimation() override;

    private:
        void ChooseDirection();
        bool IsPlayerInSight(Direction& outDirection) const;

        float m_ChaseRange;
        float m_DeathTimer{ 0.f };
        const float m_DeathDisplayTime{ 1.0f };
        bool m_ShowingDeath{ false };
    };
}