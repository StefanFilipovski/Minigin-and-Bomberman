#include "DollComponent.h"
#include "GameObject.h"
#include "GameState.h"

namespace dae {

    DollComponent::DollComponent(GameObject* owner,
        float speed,
        float moveInterval,
        std::vector<std::vector<bool>> grid,
        glm::ivec2 gridSize,
        float tileSize,
        float gridOffsetY)
        : BaseEnemyComponent(owner, speed, moveInterval, 400, // 400 points for Doll
            std::move(grid), gridSize, tileSize, gridOffsetY)
    {
      
        InitializeAnimation();
    }

    void DollComponent::InitializeAnimation()
    {
        m_Sprite->SetSpriteSheet("DollSpritesheet.tga", 1, 11, 0, 0.15f);
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::MoveDown,
            1, 11, 0, 0, 6, 0.15f, true
        );
    }

    void DollComponent::PlayDeathAnimation()
    {
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::Death,
            1, 11, 0, 6, 5, 0.2f, false
        );
    }

    void DollComponent::UpdateAI(float dt)
    {
        // Safety check
        if (!m_IsActive || GameStateManager::GetState() != GameState::Playing) {
            return;
        }

        if (m_Grid.empty()) {
            return;
        }

        m_MoveTimer -= dt;

        // Check if we need a new direction
        auto gridPos = GetGridPosition();
        bool needNewDirection = false;

        // Look ahead to avoid walls
        switch (m_CurrentDir) {
        case Direction::Left:
            needNewDirection = !IsGridPositionWalkable(gridPos.y, gridPos.x - 1);
            break;
        case Direction::Right:
            needNewDirection = !IsGridPositionWalkable(gridPos.y, gridPos.x + 1);
            break;
        case Direction::Up:
            needNewDirection = !IsGridPositionWalkable(gridPos.y - 1, gridPos.x);
            break;
        case Direction::Down:
            needNewDirection = !IsGridPositionWalkable(gridPos.y + 1, gridPos.x);
            break;
        }

        if (m_MoveTimer <= 0.f || needNewDirection) {
            ChooseRandomDirection();
            m_MoveTimer = m_MoveInterval;
        }

        float distance = m_Speed * dt;
        MoveInDirection(m_CurrentDir, distance);
    }

    void DollComponent::ChooseRandomDirection()
    {
        auto freeDirs = GetWalkableDirections();

        // Use smart direction selection for smoother movement
        m_CurrentDir = ChooseSmartDirection(freeDirs, m_CurrentDir);

        SnapToGrid(m_CurrentDir);
    }

} // namespace dae