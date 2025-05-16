#pragma once

#include "Component.h"
#include "Observer.h"
#include "TransformComponent.h"
#include "SpriteSheetComponent.h"
#include <random>

namespace dae {
    class GameObject;

    /// AI enemy that wanders via a walkability grid and notifies on death
    class BalloonComponent : public Component, public Observer, public Subject {
    public:
        enum class Direction { Left = 0, Right = 1, Up = 2, Down = 3 };

        /// @param grid    Walkability grid (copied by value)
        /// @param gridSize Width=x, Height=y of the grid
        /// @param tileSize Size of one grid cell in world units
        /// @param gridOffsetY Vertical offset applied when loading the level
        BalloonComponent(GameObject* owner,
            float speed,
            float moveInterval,
            std::vector<std::vector<bool>> grid,
            glm::ivec2 gridSize,
            float tileSize,
            float gridOffsetY);
        ~BalloonComponent() override = default;

        void Update(float dt) override;
        void OnNotify(const Event& event) override;

        void BeginMove();
        void Move(float dx, float dy);
        void RevertMove();

        /// Main AI: choose and execute movement each frame
        void MoveCurrent(float dt);
        void ChooseRandomDirection();

        /// Aligns the balloon to the grid corridor
        void SnapToGrid(Direction dir);

        /// Trigger death animation and notify score observers
        void Die();

    private:
        TransformComponent* m_Transform{ nullptr };
        SpriteSheetComponent* m_Sprite{ nullptr };

        float                                   m_Speed;
        float                                   m_MoveInterval;
        float                                   m_MoveTimer;
        Direction                               m_CurrentDir;
        bool                                    m_JustSpawned;
        bool                                    m_IsDead;
        glm::vec3                               m_LastValidPos;

        // ------------ Grid for AI pathing ------------
        std::vector<std::vector<bool>>          m_Grid;
        glm::ivec2                              m_GridSize;
        float                                   m_TileSize;
        float                                   m_GridOffsetY;

        std::mt19937                            m_Rng;
        std::uniform_int_distribution<int>      m_DirDist;
    };
}
