#pragma once

#include "Component.h"
#include "Observer.h"
#include "TransformComponent.h"
#include "SpriteSheetComponent.h"
#include <vector>
#include <random>

namespace dae {
    class GameObject;

    class BaseEnemyComponent : public Component, public Observer, public Subject {
    public:
        enum class Direction { Left = 0, Right = 1, Up = 2, Down = 3 };

        BaseEnemyComponent(GameObject* owner,
            float speed,
            float moveInterval,
            int scoreValue,
            std::vector<std::vector<bool>> grid,
            glm::ivec2 gridSize,
            float tileSize,
            float gridOffsetY);

        virtual ~BaseEnemyComponent() = default;

        void Update(float dt) override;
        void OnNotify(const Event& event) override;

        // Movement methods
        void BeginMove();
        void Move(float dx, float dy);
        void RevertMove();
        void SnapToGrid(Direction dir);

        // Death handling
        virtual void Die();
        bool IsDead() const { return m_IsDead; }

        // Grid utilities
        glm::ivec2 GetGridPosition() const;
        bool IsGridPositionWalkable(int row, int col) const;
        std::vector<Direction> GetWalkableDirections() const;

    protected:
        // Pure virtual - each enemy implements its own AI
        virtual void UpdateAI(float dt) = 0;
        virtual void InitializeAnimation() = 0;
        virtual void PlayDeathAnimation() = 0;

        // Movement execution
        void MoveInDirection(Direction dir, float distance);
        
        // Smarter direction selection
        Direction GetOppositeDirection(Direction dir) const;
        Direction ChooseSmartDirection(const std::vector<Direction>& availableDirs, Direction currentDir) const;

        // Common members
        TransformComponent* m_Transform{ nullptr };
        SpriteSheetComponent* m_Sprite{ nullptr };

        float m_Speed;
        float m_MoveInterval;
        float m_MoveTimer;
        Direction m_CurrentDir;
        bool m_JustSpawned;
        bool m_IsDead;
        glm::vec3 m_LastValidPos;
        int m_ScoreValue{ 100 };
        // Grid navigation
        std::vector<std::vector<bool>> m_Grid;
        glm::ivec2 m_GridSize;
        float m_TileSize;
        float m_GridOffsetY;

        // Random number generation
        std::mt19937 m_Rng;

        // Sprite offsets (can be overridden by derived classes)
        float m_SpriteOffsetX = 8.f;
        float m_SpriteOffsetY = 8.f;
    };
}