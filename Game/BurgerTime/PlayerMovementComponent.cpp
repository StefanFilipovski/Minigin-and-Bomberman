#include "PlayerMovementComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteSheetComponent.h"

#include <geometric.hpp>   // glm::length, glm::normalize
#include <iostream>

namespace dae {
    static constexpr float kUIOffsetY = 40.0f;

    PlayerMovementComponent::PlayerMovementComponent(
        GameObject* owner,
        const std::vector<std::vector<bool>>& platforms,
        const std::vector<std::vector<bool>>& ladders,
        int tileSize,
        int startGridX,
        int startGridY)
        : Component(owner)
        , m_platforms(platforms)
        , m_ladders(ladders)
        , m_tileSize(tileSize)
        , m_gridPos(static_cast<float>(startGridX), static_cast<float>(startGridY))
    {
        // Initial world & target positions (top-left origin, tile-sized grid)
        m_worldPos = m_gridPos * static_cast<float>(m_tileSize);
        m_targetPos = m_worldPos;

        CheckPlatformLadderState();

        if (auto* t = GetOwner()->GetComponent<TransformComponent>()) {
            t->SetLocalPosition(m_worldPos.x, m_worldPos.y, 0.0f);
        }
    }

    void PlayerMovementComponent::Update(float deltaTime) {
        // If there’s input and we’re idle, try to start a new step
        if (m_inputDirection != Direction::None && !m_isMoving) {
            if (CanMoveInDirection(m_inputDirection)) {
                m_currentDirection = m_inputDirection;
                m_isMoving = true;

                const glm::vec2 dir = GetDirectionVector(m_currentDirection);
                m_targetPos = m_worldPos + dir * static_cast<float>(m_tileSize);
            }
        }

        // Progress an active step
        if (m_isMoving) {
            const glm::vec2 diff = m_targetPos - m_worldPos;
            const float dist = glm::length(diff);

            if (dist < 1.0f) {
                // Snap to target, finish the tile
                m_worldPos = m_targetPos;
                m_isMoving = false;

                UpdateGridPosition();
                CheckPlatformLadderState();

                if (auto* sprite = GetOwner()->GetComponent<SpriteSheetComponent>()) {
                    // Idle frame (tweak to your sheet)
                    sprite->SetIdleFrame(SpriteSheetComponent::AnimationState::Idle, 2, 9, 0, 4);
                }
            }
            else {
                const glm::vec2 stepDir = diff / dist; // normalized
                m_worldPos += stepDir * m_moveSpeed * deltaTime;

                if (auto* sprite = GetOwner()->GetComponent<SpriteSheetComponent>()) {
                    switch (m_currentDirection) {
                    case Direction::Left:
                        sprite->ChangeAnimation(SpriteSheetComponent::AnimationState::MoveLeft,
                            2, 9, 0, 3, 3, 0.1f, true);
                        sprite->SetFlipHorizontal(false);
                        break;
                    case Direction::Right:
                        sprite->ChangeAnimation(SpriteSheetComponent::AnimationState::MoveRight,
                            2, 9, 0, 3, 3, 0.1f, true);
                        sprite->SetFlipHorizontal(true);
                        break;
                    case Direction::Up:
                        sprite->ChangeAnimation(SpriteSheetComponent::AnimationState::MoveUp,
                            2, 9, 0, 6, 3, 0.1f, true);
                        sprite->SetFlipHorizontal(false);
                        break;
                    case Direction::Down:
                        sprite->ChangeAnimation(SpriteSheetComponent::AnimationState::MoveDown,
                            2, 9, 0, 0, 3, 0.1f, true);
                        sprite->SetFlipHorizontal(false);
                        break;
                    case Direction::None:
                        break;
                    }
                }
            }
        }

        // Push to Transform
        if (auto* t = GetOwner()->GetComponent<TransformComponent>()) {
            t->SetLocalPosition(m_worldPos.x, m_worldPos.y + kUIOffsetY, 0.0f);
        }
    }

    void PlayerMovementComponent::PlayDeathAnimation() {
        if (auto* sprite = GetOwner()->GetComponent<SpriteSheetComponent>()) {
            // Death animation: row 1 across 9 columns (adjust indices to your sheet)
            sprite->ChangeAnimation(SpriteSheetComponent::AnimationState::Death,
                2, 9, 1, 0, 9, 0.15f, false);
            sprite->SetFlipHorizontal(false);
        }

        m_isMoving = false;
        m_inputDirection = Direction::None;
        m_currentDirection = Direction::None;
    }

    void PlayerMovementComponent::StartMove(Direction dir) {
        m_inputDirection = dir;
    }

    void PlayerMovementComponent::StopMove() {
        m_inputDirection = Direction::None;
        if (!m_isMoving) {
            m_currentDirection = Direction::None;
        }
    }

    void PlayerMovementComponent::UpdateGridPosition() {
        // World pixels -> grid coords
        m_gridPos = m_worldPos / static_cast<float>(m_tileSize);
    }

    void PlayerMovementComponent::CheckPlatformLadderState() {
        const int gx = static_cast<int>(m_gridPos.x);
        const int gy = static_cast<int>(m_gridPos.y);

        // Platforms: current tile
        m_onPlatform = false;
        if (gy >= 0 && gy < static_cast<int>(m_platforms.size())) {
            const int w = static_cast<int>(m_platforms[gy].size());
            if (gx >= 0 && gx < w) m_onPlatform = m_platforms[gy][gx];
        }
        // Or the tile below counts as “standing on floor”
        if (!m_onPlatform) {
            const int gyBelow = gy + 1;
            if (gyBelow >= 0 && gyBelow < static_cast<int>(m_platforms.size())) {
                const int wBelow = static_cast<int>(m_platforms[gyBelow].size());
                if (gx >= 0 && gx < wBelow) m_onPlatform = m_platforms[gyBelow][gx];
            }
        }

        // Ladders (unchanged)
        m_onLadder = false;
        m_canClimb = false;
        if (gy >= 0 && gy < static_cast<int>(m_ladders.size())) {
            const int w = static_cast<int>(m_ladders[gy].size());
            if (gx >= 0 && gx < w) {
                m_onLadder = m_ladders[gy][gx];
                m_canClimb = m_onLadder;
            }
        }
    }


    bool PlayerMovementComponent::CanMoveInDirection(Direction dir) const {
        if (m_isMoving) return false;

        const int gx = static_cast<int>(m_gridPos.x);
        const int gy = static_cast<int>(m_gridPos.y);

        auto rowWidthP = [&](int y) -> int {
            return (y >= 0 && y < static_cast<int>(m_platforms.size()))
                ? static_cast<int>(m_platforms[y].size())
                : 0;
            };
        auto rowWidthL = [&](int y) -> int {
            return (y >= 0 && y < static_cast<int>(m_ladders.size()))
                ? static_cast<int>(m_ladders[y].size())
                : 0;
            };

        switch (dir) {
        case Direction::Left:
            // allow horizontal if "on platform" (current or below) and within row bounds
            return m_onPlatform && gx - 1 >= 0 && gx < rowWidthP(gy);

        case Direction::Right:
            return m_onPlatform && gx + 1 < rowWidthP(gy);

        case Direction::Up:
            // require ladder in current tile (optional: also check ladder above)
            return m_canClimb && gy - 1 >= 0 && gx < rowWidthL(gy);

        case Direction::Down:
            if (m_onLadder)
                return gy + 1 < static_cast<int>(m_ladders.size()) && gx < rowWidthL(gy + 1);
            return (gy + 1 < static_cast<int>(m_ladders.size()))
                && (gx >= 0 && gx < rowWidthL(gy + 1))
                && m_ladders[gy + 1][gx];

        case Direction::None:
        default:
            return false;
        }
    }


} // namespace dae
