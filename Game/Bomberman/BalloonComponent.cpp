#include "BalloonComponent.h"
#include "GameObject.h"
#include "CollisionManager.h"
#include <cassert>
#include <cmath>
#include "GameEvents.h"
#include "CollisionComponent.h"        
#include "StaticWallResponder.h"      
#include "DestructibleWallResponder.h"
#include "CollisionUtilities.h"
#include <vector>

namespace dae {

    BalloonComponent::BalloonComponent(GameObject* owner,
        float speed,
        float moveInterval,
        std::vector<std::vector<bool>> grid,
        glm::ivec2 gridSize,
        float tileSize,
        float gridOffsetY)
        : Component(owner)
        , Observer()
        , Subject()
        , m_Speed(speed)
        , m_MoveInterval(moveInterval)
        , m_MoveTimer(moveInterval)
        , m_Grid(std::move(grid))
        , m_GridSize(gridSize)
        , m_TileSize(tileSize)
        , m_GridOffsetY(gridOffsetY)
        , m_CurrentDir(Direction::Down)
        , m_JustSpawned(true)
        , m_IsDead(false)
        , m_Rng(std::random_device{}())
        , m_DirDist(0, 3)
    {
        m_Transform = &owner->GetTransform();
        m_Sprite = owner->GetComponent<SpriteSheetComponent>();
        assert(m_Transform && m_Sprite && "Balloon needs Transform + SpriteSheetComponent");

        m_LastValidPos = m_Transform->GetLocalPosition();

        // Initialise spritesheet: frames 0-5 loop
        m_Sprite->SetSpriteSheet("BalloomSpritesheet.tga",
            /*rows=*/1, /*cols=*/11,
            /*targetRow=*/0, /*frameTime=*/0.15f);
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::MoveDown,
            /*rows=*/1, /*cols=*/11,
            /*frameRow=*/0, /*startCol=*/0,
            /*frameCount=*/6, /*frameTime=*/0.15f,
            /*loop=*/true
        );
    }

    void BalloonComponent::Update(float dt)
    {
        if (m_IsDead) {
            if (m_Sprite->GetCurrentFrame() == m_Sprite->GetFrameCount() - 1)
                m_Sprite->Hide();
            return;
        }

        m_MoveTimer -= dt;
        if (m_MoveTimer <= 0.f) {
            ChooseRandomDirection();
            m_MoveTimer += m_MoveInterval;
        }
        MoveCurrent(dt);
    }

    void BalloonComponent::MoveCurrent(float dt)
    {
        BeginMove();
        float distance = m_Speed * dt;
        switch (m_CurrentDir) {
        case Direction::Left:  Move(-distance, 0.f); break;
        case Direction::Right: Move(+distance, 0.f); break;
        case Direction::Up:    Move(0.f, -distance); break;
        case Direction::Down:  Move(0.f, +distance); break;
        }
        if (m_JustSpawned) m_JustSpawned = false;
        /*else CollisionManager::GetInstance().CheckCollisions();*/
    }

    void BalloonComponent::BeginMove()
    {
        m_LastValidPos = m_Transform->GetLocalPosition();
    }

    void BalloonComponent::Move(float dx, float dy)
    {
        auto p = m_Transform->GetLocalPosition();
        m_Transform->SetLocalPosition(p.x + dx, p.y + dy, p.z);
    }

    void BalloonComponent::RevertMove()
    {
        m_Transform->SetLocalPosition(m_LastValidPos);
    }

    void BalloonComponent::SnapToGrid(Direction dir)
    {
        constexpr float offX = 8.f, offY = 8.f;
        glm::vec3 pos = m_Transform->GetLocalPosition();
        pos.x -= offX;
        pos.y -= offY;
        // Y also needs removal of UI offset when recalc grid, but here we just recenter
        if (dir == Direction::Left || dir == Direction::Right)
            pos.y = std::floor(pos.y / m_TileSize + 0.5f) * m_TileSize;
        else
            pos.x = std::floor(pos.x / m_TileSize + 0.5f) * m_TileSize;
        pos.x += offX;
        pos.y += offY;
        m_Transform->SetLocalPosition(pos);
        m_LastValidPos = pos;
    }

    void BalloonComponent::ChooseRandomDirection()
    {
        constexpr float offX = 8.f, offY = 8.f;
        auto pos = m_Transform->GetLocalPosition();
        float localX = pos.x - offX;
        float localY = pos.y - offY - m_GridOffsetY;
        int col = static_cast<int>(localX / m_TileSize);
        int row = static_cast<int>(localY / m_TileSize);

        std::vector<Direction> freeDirs;
        auto test = [&](Direction d, int dr, int dc) {
            int nr = row + dr;
            int nc = col + dc;
            if (nr < 0 || nr >= m_GridSize.y) return;
            if (nc < 0 || nc >= m_GridSize.x) return;
            if (m_Grid[nr][nc]) freeDirs.push_back(d);
            };
        test(Direction::Left, 0, -1);
        test(Direction::Right, 0, +1);
        test(Direction::Up, -1, 0);
        test(Direction::Down, +1, 0);

        if (freeDirs.empty()) {
            switch (m_CurrentDir) {
            case Direction::Left:  m_CurrentDir = Direction::Right; break;
            case Direction::Right: m_CurrentDir = Direction::Left;  break;
            case Direction::Up:    m_CurrentDir = Direction::Down;  break;
            case Direction::Down:  m_CurrentDir = Direction::Up;    break;
            }
        }
        else {
            std::uniform_int_distribution<int> dist(0, static_cast<int>(freeDirs.size()) - 1);
            m_CurrentDir = freeDirs[dist(m_Rng)];
        }

        SnapToGrid(m_CurrentDir);
    }

    void BalloonComponent::Die()
    {
        if (m_IsDead) return;
        m_IsDead = true;
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::Death,
            /*rows=*/1, /*cols=*/11,
            /*frameRow=*/0, /*startCol=*/6,
            /*frameCount=*/5, /*frameTime=*/0.2f,
            /*loop=*/false
        );
        Notify({ GameEvents::POINTS_ADDED });
    }

    void BalloonComponent::OnNotify(const Event& event) {
        (void)event;
    }

} // namespace dae