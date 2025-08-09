#pragma once
#include "Component.h"
#include "Direction.h"              // single source of truth for Direction
#include <vec2.hpp>
#include <vector>

namespace dae {

    class PlayerMovementComponent : public Component {
    public:
        PlayerMovementComponent(
            GameObject* owner,
            const std::vector<std::vector<bool>>& platforms,
            const std::vector<std::vector<bool>>& ladders,
            int tileSize,
            int startGridX,
            int startGridY
        );

        void Update(float deltaTime) override;

        // Movement input
        void StartMove(Direction dir);
        void StopMove();

        // State / control
        void PlayDeathAnimation();
        Direction GetCurrentDirection() const { return m_currentDirection; }
        bool IsOnPlatform() const { return m_onPlatform; }
        bool IsOnLadder()   const { return m_onLadder; }
        bool CanClimb()     const { return m_canClimb; }
        void SetMoveSpeed(float pixelsPerSecond) { m_moveSpeed = pixelsPerSecond; }

    private:
        // References to collision grids
        const std::vector<std::vector<bool>>& m_platforms;
        const std::vector<std::vector<bool>>& m_ladders;
        int m_tileSize;

        // Positions
        glm::vec2 m_gridPos{ 0.f, 0.f };     // grid coords (floats while moving)
        glm::vec2 m_targetPos{ 0.f, 0.f };   // target world position for current step
        glm::vec2 m_worldPos{ 0.f, 0.f };    // current world position (pixels)

        // Movement state
        Direction m_currentDirection = Direction::None;
        Direction m_inputDirection = Direction::None;
        bool  m_isMoving = false;
        float m_moveSpeed = 80.0f;         // pixels/sec

        // Environment state
        bool m_onPlatform = false;
        bool m_onLadder = false;
        bool m_canClimb = false;

        // Helpers
        void UpdateGridPosition();
        void CheckPlatformLadderState();
        bool CanMoveInDirection(Direction dir) const;
        glm::vec2 GetDirectionVector(Direction dir) const {
            switch (dir) {
            case Direction::Left:  return { -1.f,  0.f };
            case Direction::Right: return { 1.f,  0.f };
            case Direction::Up:    return { 0.f, -1.f };
            case Direction::Down:  return { 0.f,  1.f };
            case Direction::None:
            default:               return { 0.f,  0.f };
            }
        }
    };

} // namespace dae
