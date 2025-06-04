#include "BalloonComponent.h"
#include "GameObject.h"

namespace dae {

    BalloonComponent::BalloonComponent(GameObject* owner,
        float speed,
        float moveInterval,
        std::vector<std::vector<bool>> grid,
        glm::ivec2 gridSize,
        float tileSize,
        float gridOffsetY)
        : BaseEnemyComponent(owner, speed, moveInterval, 100, // 100 points for balloon
            std::move(grid), gridSize, tileSize, gridOffsetY)
        , m_DirDist(0, 3)
    {
        InitializeAnimation();
    }

    void BalloonComponent::InitializeAnimation()
    {
        m_Sprite->SetSpriteSheet("BalloomSpritesheet.tga", 1, 11, 0, 0.15f);
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::MoveDown,
            1, 11, 0, 0, 6, 0.15f, true
        );
    }

    void BalloonComponent::PlayDeathAnimation()
    {
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::Death,
            1, 11, 0, 6, 5, 0.2f, false
        );
    }

    void BalloonComponent::UpdateAI(float dt)
    {
        m_MoveTimer -= dt;
        if (m_MoveTimer <= 0.f) {
            ChooseRandomDirection();
            m_MoveTimer += m_MoveInterval;
        }

        float distance = m_Speed * dt;
        MoveInDirection(m_CurrentDir, distance);
    }

    void BalloonComponent::ChooseRandomDirection()
    {
        auto freeDirs = GetWalkableDirections();

        m_CurrentDir = ChooseSmartDirection(freeDirs, m_CurrentDir);

        SnapToGrid(m_CurrentDir);
    }

} // namespace dae