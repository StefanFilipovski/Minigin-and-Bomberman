#include "BaseEnemyComponent.h"
#include "GameObject.h"
#include "GameEvents.h"
#include <cassert>
#include <cmath>
#include <CollisionComponent.h>
#include <ServiceLocator.h>
#include <SoundIds.h>
#include "GameState.h"

namespace dae {

    BaseEnemyComponent::BaseEnemyComponent(GameObject* owner,
        float speed,
        float moveInterval,
        int scoreValue,
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
        , m_ScoreValue(scoreValue)
        , m_Grid(std::move(grid))
        , m_GridSize(gridSize)
        , m_TileSize(tileSize)
        , m_GridOffsetY(gridOffsetY)
        , m_CurrentDir(Direction::Down)
        , m_JustSpawned(true)
        , m_IsDead(false)
        , m_IsActive(true)
        , m_Rng(std::random_device{}())
    {
        m_Transform = &owner->GetTransform();
        m_Sprite = owner->GetComponent<SpriteSheetComponent>();
        assert(m_Transform && m_Sprite && "Enemy needs Transform + SpriteSheetComponent");

        m_LastValidPos = m_Transform->GetLocalPosition();
        std::cout << "BaseEnemyComponent created with score: " << m_ScoreValue << std::endl;
    }

    void BaseEnemyComponent::Update(float dt)
    {
        // Don't update if we're in any transition state
        if (GameStateManager::IsTransitioning()) {
            return;
        }

        // Safety checks
        if (!m_IsActive || !GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

        // Additional check for valid grid
        if (m_Grid.empty() || m_GridSize.x <= 0 || m_GridSize.y <= 0) {
            return;
        }

        if (m_IsDead) {
            if (m_Sprite && m_Sprite->GetCurrentFrame() == m_Sprite->GetFrameCount() - 1) {
                m_Sprite->Hide();
                GetOwner()->MarkForDeletion();
            }
            return;
        }

        // Let derived class handle AI
        UpdateAI(dt);

        if (m_JustSpawned) {
            m_JustSpawned = false;
        }
    }

    void BaseEnemyComponent::Deactivate()
    {
        m_IsActive = false;
        m_IsDead = true;

        // Clear grid to prevent access
        m_Grid.clear();

        // Disable collision
        if (auto* collision = GetOwner()->GetComponent<CollisionComponent>()) {
            collision->SetResponder(nullptr);
        }
    }

    void BaseEnemyComponent::BeginMove()
    {
        m_LastValidPos = m_Transform->GetLocalPosition();
    }

    void BaseEnemyComponent::Move(float dx, float dy)
    {
        auto p = m_Transform->GetLocalPosition();
        m_Transform->SetLocalPosition(p.x + dx, p.y + dy, p.z);
    }

    void BaseEnemyComponent::RevertMove()
    {
        m_Transform->SetLocalPosition(m_LastValidPos);
    }

    void BaseEnemyComponent::MoveInDirection(Direction dir, float distance)
    {
        BeginMove();
        switch (dir) {
        case Direction::Left:  Move(-distance, 0.f); break;
        case Direction::Right: Move(+distance, 0.f); break;
        case Direction::Up:    Move(0.f, -distance); break;
        case Direction::Down:  Move(0.f, +distance); break;
        }
    }

    void BaseEnemyComponent::SnapToGrid(Direction dir)
    {
        glm::vec3 pos = m_Transform->GetLocalPosition();
        pos.x -= m_SpriteOffsetX;
        pos.y -= m_SpriteOffsetY;

        if (dir == Direction::Left || dir == Direction::Right) {
            pos.y = std::floor(pos.y / m_TileSize + 0.5f) * m_TileSize;
        }
        else {
            pos.x = std::floor(pos.x / m_TileSize + 0.5f) * m_TileSize;
        }

        pos.x += m_SpriteOffsetX;
        pos.y += m_SpriteOffsetY;
        m_Transform->SetLocalPosition(pos);
        m_LastValidPos = pos;
    }

    glm::ivec2 BaseEnemyComponent::GetGridPosition() const
    {
        auto pos = m_Transform->GetLocalPosition();
        float localX = pos.x - m_SpriteOffsetX;
        float localY = pos.y - m_SpriteOffsetY - m_GridOffsetY;
        int col = static_cast<int>(localX / m_TileSize);
        int row = static_cast<int>(localY / m_TileSize);
        return { col, row };
    }

    bool BaseEnemyComponent::IsGridPositionWalkable(int row, int col) const
    {
        // Extra safety checks
        if (m_Grid.empty() || m_GridSize.x <= 0 || m_GridSize.y <= 0) {
            return false;
        }

        if (row < 0 || row >= m_GridSize.y || col < 0 || col >= m_GridSize.x) {
            return false;
        }

        // Bounds check for the actual vector
        if (row >= static_cast<int>(m_Grid.size())) {
            return false;
        }

        if (m_Grid[row].empty() || col >= static_cast<int>(m_Grid[row].size())) {
            return false;
        }

        return m_Grid[row][col];
    }

    std::vector<BaseEnemyComponent::Direction> BaseEnemyComponent::GetWalkableDirections() const
    {
        auto gridPos = GetGridPosition();
        int row = gridPos.y;
        int col = gridPos.x;

        std::vector<Direction> freeDirs;

        if (IsGridPositionWalkable(row, col - 1)) freeDirs.push_back(Direction::Left);
        if (IsGridPositionWalkable(row, col + 1)) freeDirs.push_back(Direction::Right);
        if (IsGridPositionWalkable(row - 1, col)) freeDirs.push_back(Direction::Up);
        if (IsGridPositionWalkable(row + 1, col)) freeDirs.push_back(Direction::Down);

        return freeDirs;
    }

    BaseEnemyComponent::Direction BaseEnemyComponent::GetOppositeDirection(Direction dir) const
    {
        switch (dir) {
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        }
        return dir;
    }

    BaseEnemyComponent::Direction BaseEnemyComponent::ChooseSmartDirection(
        const std::vector<Direction>& availableDirs,
        Direction currentDir) const
    {
        if (availableDirs.empty()) {
            return GetOppositeDirection(currentDir);
        }

        // If we can continue in the same direction, prefer that (momentum)
        auto it = std::find(availableDirs.begin(), availableDirs.end(), currentDir);
        if (it != availableDirs.end()) {
            // 70% chance to continue straight if possible
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(const_cast<std::mt19937&>(m_Rng)) < 0.7f) {
                return currentDir;
            }
        }

        // Otherwise pick a random available direction
        std::uniform_int_distribution<int> dist(0, static_cast<int>(availableDirs.size()) - 1);
        return availableDirs[dist(const_cast<std::mt19937&>(m_Rng))];
    }

    void BaseEnemyComponent::Die()
    {
        if (m_IsDead || !m_IsActive) return;

        // Safety check
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

        m_IsDead = true;
        m_IsActive = false;

        std::cout << "Enemy dying with score value: " << m_ScoreValue << std::endl;

        ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_ENEMY_DIE, 0.6f);

        // Call derived class death animation
        PlayDeathAnimation();

        // Disable collision when dead
        if (auto* collision = GetOwner()->GetComponent<CollisionComponent>()) {
            collision->SetResponder(nullptr);
        }

        // Immediate notification - no delay needed if we're careful
        Event deathEvent;
        deathEvent.id = GameEvents::ENEMY_DIED;
        deathEvent.data = m_ScoreValue;
        Notify(deathEvent);
    }

    void BaseEnemyComponent::OnNotify(const Event& event)
    {
        // Base implementation - derived classes can override
        (void)event;
    }

} // namespace dae