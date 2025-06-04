#include "MinvoComponent.h"
#include "GameObject.h"
#include "PlayerManager.h"
#include <algorithm>

namespace dae {

    MinvoComponent::MinvoComponent(GameObject* owner,
        float speed,
        float moveInterval,
        float chaseRange,
        std::vector<std::vector<bool>> grid,
        glm::ivec2 gridSize,
        float tileSize,
        float gridOffsetY)
        : BaseEnemyComponent(owner, speed, moveInterval, 800, // 800 points for Minvo (highest value)
            std::move(grid), gridSize, tileSize, gridOffsetY)
        , m_ChaseRange(chaseRange)
    {
        InitializeAnimation();
    }

    void MinvoComponent::InitializeAnimation()
    {
        m_Sprite->SetSpriteSheet("MinvoSpritesheet.tga", 1, 7, 0, 0.15f);
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::MoveDown,
            1, 7, 0, 0, 6, 0.15f, true
        );
    }

    void MinvoComponent::PlayDeathAnimation()
    {
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::Death,
            1, 7, 0, 6, 1, 0.1f, false
        );
        m_ShowingDeath = true;
    }

    void MinvoComponent::UpdateAI(float dt)
    {
        // Handle death display timer
        if (m_ShowingDeath) {
            m_DeathTimer += dt;
            if (m_DeathTimer >= m_DeathDisplayTime) {
                m_Sprite->Hide();
            }
            return;
        }

        m_MoveTimer -= dt;

        // Minvo makes decisions more frequently for aggressive chasing
        if (m_MoveTimer <= 0.f) {
            ChooseDirection();
            m_MoveTimer += m_MoveInterval;
        }

        float distance = m_Speed * dt;
        MoveInDirection(m_CurrentDir, distance);
    }

    void MinvoComponent::ChooseDirection()
    {
        Direction chaseDir;

        // Check if any player is in sight and chase if so
        if (IsPlayerInSight(chaseDir)) {
            // Minvo aggressively chases - always takes chase direction if available
            auto freeDirs = GetWalkableDirections();
            auto it = std::find(freeDirs.begin(), freeDirs.end(), chaseDir);

            if (it != freeDirs.end()) {
                m_CurrentDir = chaseDir;
            }
            else {
                // Can't move directly, try to find best path
                // Minvo is smart - will try to move around obstacles
                std::vector<Direction> alternatives;

                if (chaseDir == Direction::Left || chaseDir == Direction::Right) {
                    // Try vertical movements to get around obstacle
                    if (std::find(freeDirs.begin(), freeDirs.end(), Direction::Up) != freeDirs.end())
                        alternatives.push_back(Direction::Up);
                    if (std::find(freeDirs.begin(), freeDirs.end(), Direction::Down) != freeDirs.end())
                        alternatives.push_back(Direction::Down);
                }
                else {
                    // Try horizontal movements to get around obstacle
                    if (std::find(freeDirs.begin(), freeDirs.end(), Direction::Left) != freeDirs.end())
                        alternatives.push_back(Direction::Left);
                    if (std::find(freeDirs.begin(), freeDirs.end(), Direction::Right) != freeDirs.end())
                        alternatives.push_back(Direction::Right);
                }

                if (!alternatives.empty()) {
                    std::uniform_int_distribution<int> dist(0, static_cast<int>(alternatives.size()) - 1);
                    m_CurrentDir = alternatives[dist(m_Rng)];
                }
                else {
                    // No alternatives, use smart direction
                    m_CurrentDir = ChooseSmartDirection(freeDirs, m_CurrentDir);
                }
            }

            SnapToGrid(m_CurrentDir);
            return;
        }

        // Not in chase mode - patrol smartly
        auto freeDirs = GetWalkableDirections();

        // Check ahead for dead ends and prefer directions with more options
        if (freeDirs.size() > 1) {
            // 50% chance to change direction at intersections for patrol variety
            std::uniform_real_distribution<float> changeDist(0.0f, 1.0f);
            if (changeDist(m_Rng) < 0.5f) {
                std::uniform_int_distribution<int> dist(0, static_cast<int>(freeDirs.size()) - 1);
                m_CurrentDir = freeDirs[dist(m_Rng)];
            }
            else {
                m_CurrentDir = ChooseSmartDirection(freeDirs, m_CurrentDir);
            }
        }
        else {
            m_CurrentDir = ChooseSmartDirection(freeDirs, m_CurrentDir);
        }

        SnapToGrid(m_CurrentDir);
    }

    bool MinvoComponent::IsPlayerInSight(Direction& outDirection) const
    {
        // Get the closest player from PlayerManager
        auto myPos = m_Transform->GetLocalPosition();
        GameObject* targetPlayer = PlayerManager::GetInstance().GetClosestPlayer(myPos);

        if (!targetPlayer) {
            return false;
        }

        auto myGridPos = GetGridPosition();
        auto playerTransform = &targetPlayer->GetTransform();
        auto playerPos = playerTransform->GetLocalPosition();

        // Convert player position to grid coordinates
        float playerLocalX = playerPos.x - 7.5f;
        float playerLocalY = playerPos.y - 5.0f - m_GridOffsetY;
        int playerCol = static_cast<int>(playerLocalX / m_TileSize);
        int playerRow = static_cast<int>(playerLocalY / m_TileSize);

        // Check if player is within chase range
        float dx = static_cast<float>(playerCol - myGridPos.x);
        float dy = static_cast<float>(playerRow - myGridPos.y);
        float distance = std::sqrt(dx * dx + dy * dy) * m_TileSize;

        if (distance > m_ChaseRange) {
            return false;
        }

        // Minvo has enhanced sight - can see diagonally too
        // First check cardinal directions (same as Oneal)
        if (myGridPos.y == playerRow) { // Same row
            if (playerCol < myGridPos.x) { // Player is to the left
                for (int c = myGridPos.x - 1; c >= playerCol; --c) {
                    if (!IsGridPositionWalkable(myGridPos.y, c)) return false;
                }
                outDirection = Direction::Left;
                return true;
            }
            else if (playerCol > myGridPos.x) { // Player is to the right
                for (int c = myGridPos.x + 1; c <= playerCol; ++c) {
                    if (!IsGridPositionWalkable(myGridPos.y, c)) return false;
                }
                outDirection = Direction::Right;
                return true;
            }
        }
        else if (myGridPos.x == playerCol) { // Same column
            if (playerRow < myGridPos.y) { // Player is above
                for (int r = myGridPos.y - 1; r >= playerRow; --r) {
                    if (!IsGridPositionWalkable(r, myGridPos.x)) return false;
                }
                outDirection = Direction::Up;
                return true;
            }
            else if (playerRow > myGridPos.y) { // Player is below
                for (int r = myGridPos.y + 1; r <= playerRow; ++r) {
                    if (!IsGridPositionWalkable(r, myGridPos.x)) return false;
                }
                outDirection = Direction::Down;
                return true;
            }
        }
        else {
            // Minvo can also decide direction based on closest axis
            // This makes Minvo smarter at finding players
            if (std::abs(dx) > std::abs(dy)) {
                // Horizontal is closer
                outDirection = (playerCol < myGridPos.x) ? Direction::Left : Direction::Right;
            }
            else {
                // Vertical is closer
                outDirection = (playerRow < myGridPos.y) ? Direction::Up : Direction::Down;
            }
            return true;
        }

        return false;
    }

} // namespace dae