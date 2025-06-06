#include "PlayerComponent.h"
#include "GameObject.h"
#include "SpriteSheetComponent.h"
#include "TransformComponent.h"
#include "GameEvents.h"
#include <cassert>
#include <iostream>
#include <ServiceLocator.h>
#include "CollisionManager.h"
#include "BombComponent.h"
#include "StaticWallResponder.h"
#include <CollisionUtilities.h>
#include "DestructibleWallResponder.h"
#include "CollisionComponent.h"
#include "Scene.h"
namespace dae {

    PlayerComponent::PlayerComponent(GameObject* owner)
        : Component(owner)
    {
        m_Transform = &owner->GetTransform();
        m_Sprite = owner->GetComponent<SpriteSheetComponent>();
        if (!m_Sprite) {
            for (auto* child : owner->GetChildren()) {
                m_Sprite = child->GetComponent<SpriteSheetComponent>();
                if (m_Sprite) break;
            }
        }
        assert(m_Transform && m_Sprite && "PlayerComponent needs Transform + SpriteSheet");
        m_lastValidPosition = m_Transform->GetLocalPosition();
    }

    void PlayerComponent::Update(float dt)
    {
        // Invul + blink
        if (m_IsInvulnerable) {
            m_InvulTimer -= dt;
            m_FlashTimer -= dt;
            if (m_FlashTimer <= 0.f) {
                m_FlashTimer += m_FlashInterval;
                if (m_SpriteVisible) { m_Sprite->Hide(); m_SpriteVisible = false; }
                else { m_Sprite->Show(); m_SpriteVisible = true; }
            }
            if (m_InvulTimer <= 0.f) {
                m_IsInvulnerable = false;
                m_Sprite->Show();
                m_SpriteVisible = true;
            }
        }

        if (!m_IsDead) {
            MoveCurrent(dt);
        }
        else {
            // hide after death anim finishes
            if (m_Sprite->GetFrameCount() > 0 &&
                m_Sprite->GetCurrentFrame() == m_Sprite->GetFrameCount() - 1)
            {
                m_Sprite->Hide();
            }
        }
    }

    int PlayerComponent::GetHealth() const { return m_health; }

    void PlayerComponent::TakeDamage(int dmg)
    {
        if (m_IsDead || m_IsInvulnerable) return;
        m_health -= dmg;
        std::cout << "Player took " << dmg << " damage, health now " << m_health << "\n";
        ServiceLocator::GetSoundSystem().Play(0, 1.0f);
        Notify({ GameEvents::PLAYER_HIT });
        if (m_health <= 0) {
            m_IsDead = true;
            Notify({ GameEvents::PLAYER_DIED });
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::Death,
                3, 6, 2, 0, 6, 0.15f, false
            );
        }
        else {
            m_IsInvulnerable = true;
            m_InvulTimer = m_InvulDuration;
            m_FlashTimer = m_FlashInterval;
            m_SpriteVisible = true;
        }
    }

    void PlayerComponent::OnMovementPressed(Direction dir)
    {
        if (m_IsDead) return;
        constexpr float offsetX = 8.5f, offsetY = 5.f;
        auto wp = GetOwner()->GetTransform().GetWorldPosition();
        wp.x -= offsetX; wp.y -= offsetY;
        if (dir == Direction::Up || dir == Direction::Down)
            wp.x = std::floor(wp.x / s_TileSize + 0.5f) * s_TileSize;
        else
            wp.y = std::floor(wp.y / s_TileSize + 0.5f) * s_TileSize;
        wp.x += offsetX; wp.y += offsetY;
        GetOwner()->GetTransform().SetLocalPosition(wp.x, wp.y, wp.z);
        m_lastValidPosition = GetOwner()->GetTransform().GetLocalPosition();
        if (std::find(m_MovementDirs.begin(), m_MovementDirs.end(), dir) == m_MovementDirs.end()) {
            m_MovementDirs.push_back(dir);
        }
        UpdateSpriteState();
    }

    void PlayerComponent::OnMovementReleased(Direction dir)
    {
        if (m_IsDead) return;
        auto it = std::find(m_MovementDirs.begin(), m_MovementDirs.end(), dir);
        if (it != m_MovementDirs.end()) {
            m_MovementDirs.erase(it);
        }
        UpdateSpriteState();
    }

    void PlayerComponent::BeginMove()
    {
        m_lastValidPosition = m_Transform->GetLocalPosition();
    }

    void PlayerComponent::Move(float dx, float dy)
    {
        auto pos = m_Transform->GetLocalPosition();
        m_Transform->SetLocalPosition(pos.x + dx, pos.y + dy, pos.z);
    }

    void PlayerComponent::RevertMove()
    {
        m_Transform->SetLocalPosition(
            m_lastValidPosition.x,
            m_lastValidPosition.y,
            m_lastValidPosition.z
        );
    }

    void PlayerComponent::MoveCurrent(float dt)
    {
        if (m_MovementDirs.empty()) return;
        float amount = m_speed * dt;
        BeginMove();
        switch (m_MovementDirs.back()) {
        case Direction::Left:  Move(-amount, 0.f); break;
        case Direction::Right: Move(amount, 0.f); break;
        case Direction::Up:    Move(0.f, -amount); break;
        case Direction::Down:  Move(0.f, amount); break;
        }
        if (m_justSpawned) {
            m_justSpawned = false;
        }
       
    }

    void PlayerComponent::UpdateSpriteState()
    {
        if (m_MovementDirs.empty()) {
            m_Sprite->SetIdleFrame(
                SpriteSheetComponent::AnimationState::Idle,
                3, 6, 0, 4
            );
            return;
        }
        constexpr float FD = 0.15f;
        switch (m_MovementDirs.back()) {
        case Direction::Left:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveLeft,
                3, 6, 0, 0, 3, FD, true
            );
            break;
        case Direction::Down:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveDown,
                3, 6, 0, 3, 3, FD, true
            );
            break;
        case Direction::Right:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveRight,
                3, 6, 1, 0, 3, FD, true
            );
            break;
        case Direction::Up:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveUp,
                3, 6, 1, 3, 3, FD, true
            );
            break;
        }
    }
   

    void PlayerComponent::PlaceBomb(Scene& scene)
    {
        // Check if we can place a bomb
        if (!CanPlaceBomb()) {
            return; // Already at max bombs
        }

        constexpr float bombOffsetX = 8.f;
        constexpr float bombOffsetY = 8.f;
        auto raw = GetOwner()->GetTransform().GetWorldPosition();
        raw.x -= bombOffsetX;
        raw.y -= bombOffsetY;
        glm::vec2 gridPos{
            std::floor(raw.x / s_TileSize + 0.5f) * s_TileSize,
            std::floor(raw.y / s_TileSize + 0.5f) * s_TileSize
        };
        SDL_Rect bombBox{ int(gridPos.x), int(gridPos.y),
                         int(s_TileSize),  int(s_TileSize) };
        for (auto* c : CollisionManager::GetInstance().GetComponents()) {
            auto* r = c->GetResponder();
            if ((dynamic_cast<StaticWallResponder*>(r) ||
                dynamic_cast<DestructibleWallResponder*>(r)) &&
                AABBIntersect(bombBox, c->GetBoundingBox()))
            {
                return; // blocked
            }
        }
        auto bombGO = std::make_shared<GameObject>();
        bombGO->AddComponent<TransformComponent>()
            .SetLocalPosition(gridPos.x + bombOffsetX,
                gridPos.y + bombOffsetY,
                0.f);
        auto& bc = bombGO->AddComponent<BombComponent>();
        bc.Init("BombSpritesheet.tga", 3, 1, 0.2f, m_BombRange, /*fuse=*/2.f, scene);

        // Subscribe to bomb events
        bc.AddObserver(this);

        // Track bomb for detonator
        m_ActiveBombs.push_back(&bc);

        m_ActiveBombCount++;

        scene.Add(bombGO);
    }

    void PlayerComponent::DetonateOldestBomb()
    {
        if (!m_HasDetonator || m_ActiveBombs.empty()) {
            return;
        }

        // Find the first bomb that hasn't exploded yet
        BombComponent* bombToDetonate = nullptr;
        for (auto* bomb : m_ActiveBombs) {
            if (bomb && !bomb->IsExploded()) {
                bombToDetonate = bomb;
                break;
            }
        }

        // Detonate outside the loop to avoid iterator issues
        if (bombToDetonate) {
            bombToDetonate->ForceExplode();
            // Don't remove from vector here - let OnNotify handle it
        }
    }

    void PlayerComponent::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::BOMB_EXPLODED) {
            if (m_ActiveBombCount > 0) {
                m_ActiveBombCount--;
            }

            // Create a new vector with only valid bombs
            std::vector<BombComponent*> validBombs;
            for (auto* bomb : m_ActiveBombs) {
                if (bomb && !bomb->IsExploded()) {
                    validBombs.push_back(bomb);
                }
            }

            // Replace the old vector
            m_ActiveBombs = std::move(validBombs);
        }
    }

} // namespace dae

