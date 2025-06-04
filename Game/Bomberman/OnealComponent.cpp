#include "OnealComponent.h"
#include "GameObject.h"
#include "PlayerManager.h"
#include <algorithm>
#include <iostream>

namespace dae {

    OnealComponent::OnealComponent(GameObject* owner,
        float speed,
        float moveInterval,
        float chaseRange,
        std::vector<std::vector<bool>> grid,
        glm::ivec2 gridSize,
        float tileSize,
        float gridOffsetY)
        : BaseEnemyComponent(owner, speed, moveInterval, 200, // 200 points for Oneal
            std::move(grid), gridSize, tileSize, gridOffsetY)
        , m_ChaseRange(chaseRange)
    {
        InitializeAnimation();
        std::cout << "Oneal created with chase range: " << m_ChaseRange << std::endl;
    }

    void OnealComponent::InitializeAnimation()
    {
        m_Sprite->SetSpriteSheet("OnealSpritesheet.tga", 1, 7, 0, 0.15f);
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::MoveDown,
            1, 7, 0, 0, 6, 0.15f, true
        );
    }

    void OnealComponent::PlayDeathAnimation()
    {
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::Death,
            1, 7, 0, 6, 1, 0.1f, false
        );
        m_ShowingDeath = true;
    }

    void OnealComponent::UpdateAI(float dt)
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
        if (m_MoveTimer <= 0.f) {
            ChooseDirection();
            m_MoveTimer += m_MoveInterval;
        }

        float distance = m_Speed * dt;
        MoveInDirection(m_CurrentDir, distance);
    }

    void OnealComponent::ChooseDirection()
    {
        Direction chaseDir;

        // Check if any player is in sight and chase if so
        if (IsPlayerInSight(chaseDir)) {
            // When chasing, check if the chase direction is actually walkable
            auto freeDirs = GetWalkableDirections();
            auto it = std::find(freeDirs.begin(), freeDirs.end(), chaseDir);

            if (it != freeDirs.end()) {
                // Can move in chase direction
                m_CurrentDir = chaseDir;
            }
            else {
                // Can't move directly toward player, choose best alternative
                // Try perpendicular directions first
                std::vector<Direction> alternatives;

                if (chaseDir == Direction::Left || chaseDir == Direction::Right) {
                    if (std::find(freeDirs.begin(), freeDirs.end(), Direction::Up) != freeDirs.end())
                        alternatives.push_back(Direction::Up);
                    if (std::find(freeDirs.begin(), freeDirs.end(), Direction::Down) != freeDirs.end())
                        alternatives.push_back(Direction::Down);
                }
                else {
                    if (std::find(freeDirs.begin(), freeDirs.end(), Direction::Left) != freeDirs.end())
                        alternatives.push_back(Direction::Left);
                    if (std::find(freeDirs.begin(), freeDirs.end(), Direction::Right) != freeDirs.end())
                        alternatives.push_back(Direction::Right);
                }

                if (!alternatives.empty()) {
                    // Pick a perpendicular direction
                    std::uniform_int_distribution<int> dist(0, static_cast<int>(alternatives.size()) - 1);
                    m_CurrentDir = alternatives[dist(m_Rng)];
                }
                else {
                    // No good alternatives, use smart direction
                    m_CurrentDir = ChooseSmartDirection(freeDirs, m_CurrentDir);
                }
            }

            SnapToGrid(m_CurrentDir);
            return;
        }

        // Otherwise move using smart direction selection
        auto freeDirs = GetWalkableDirections();
        m_CurrentDir = ChooseSmartDirection(freeDirs, m_CurrentDir);
        SnapToGrid(m_CurrentDir);
    }

    bool OnealComponent::IsPlayerInSight(Direction& outDirection) const
    {
        // Get the closest player from PlayerManager
        auto myPos = m_Transform->GetLocalPosition();
        GameObject* targetPlayer = PlayerManager::GetInstance().GetClosestPlayer(myPos);

        if (!targetPlayer) {
            std::cout << "No player found in PlayerManager!" << std::endl;
            return false;
        }

        auto myGridPos = GetGridPosition();
        auto playerTransform = &targetPlayer->GetTransform();
        auto playerPos = playerTransform->GetLocalPosition();

        // Debug output
        std::cout << "Oneal at grid: " << myGridPos.x << ", " << myGridPos.y
            << " world: " << myPos.x << ", " << myPos.y << std::endl;

        // Convert player position to grid coordinates
        // Player has offset of 7.5, 5 added to its position (from LevelLoader)
        float playerLocalX = playerPos.x - 7.5f;
        float playerLocalY = playerPos.y - 5.0f - m_GridOffsetY;
        int playerCol = static_cast<int>(playerLocalX / m_TileSize);
        int playerRow = static_cast<int>(playerLocalY / m_TileSize);

        std::cout << "Player at grid: " << playerCol << ", " << playerRow
            << " world: " << playerPos.x << ", " << playerPos.y << std::endl;

        // Check if player is within chase range
        float dx = static_cast<float>(playerCol - myGridPos.x);
        float dy = static_cast<float>(playerRow - myGridPos.y);
        float distance = std::sqrt(dx * dx + dy * dy) * m_TileSize;

        std::cout << "Distance to player: " << distance << " (chase range: " << m_ChaseRange << ")" << std::endl;

        if (distance > m_ChaseRange) {
            return false;
        }

        // Check line of sight in cardinal directions
        if (myGridPos.y == playerRow) { // Same row
            if (playerCol < myGridPos.x) { // Player is to the left
                for (int c = myGridPos.x - 1; c >= playerCol; --c) {
                    if (!IsGridPositionWalkable(myGridPos.y, c)) {
                        std::cout << "Path blocked at col " << c << std::endl;
                        return false;
                    }
                }
                outDirection = Direction::Left;
                std::cout << "Player detected to the left!" << std::endl;
                return true;
            }
            else if (playerCol > myGridPos.x) { // Player is to the right
                for (int c = myGridPos.x + 1; c <= playerCol; ++c) {
                    if (!IsGridPositionWalkable(myGridPos.y, c)) {
                        std::cout << "Path blocked at col " << c << std::endl;
                        return false;
                    }
                }
                outDirection = Direction::Right;
                std::cout << "Player detected to the right!" << std::endl;
                return true;
            }
        }
        else if (myGridPos.x == playerCol) { // Same column
            if (playerRow < myGridPos.y) { // Player is above
                for (int r = myGridPos.y - 1; r >= playerRow; --r) {
                    if (!IsGridPositionWalkable(r, myGridPos.x)) {
                        std::cout << "Path blocked at row " << r << std::endl;
                        return false;
                    }
                }
                outDirection = Direction::Up;
                std::cout << "Player detected above!" << std::endl;
                return true;
            }
            else if (playerRow > myGridPos.y) { // Player is below
                for (int r = myGridPos.y + 1; r <= playerRow; ++r) {
                    if (!IsGridPositionWalkable(r, myGridPos.x)) {
                        std::cout << "Path blocked at row " << r << std::endl;
                        return false;
                    }
                }
                outDirection = Direction::Down;
                std::cout << "Player detected below!" << std::endl;
                return true;
            }
        }

        return false;
    }

} // namespace dae