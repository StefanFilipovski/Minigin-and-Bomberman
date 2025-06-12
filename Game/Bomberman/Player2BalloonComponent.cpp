#include "Player2BalloonComponent.h"
#include "GameObject.h"
#include "Scene.h"
#include "GameEvents.h"
#include "ServiceLocator.h"
#include "SoundIds.h"
#include "CollisionManager.h"
#include "CollisionComponent.h"
#include "StaticWallResponder.h"
#include "DestructibleWallResponder.h"
#include "BombCollisionResponder.h"
#include "CollisionUtilities.h"
#include "VersusGameController.h"
#include <cassert>
#include <iostream>

namespace dae {
    Player2BalloonComponent::Player2BalloonComponent(GameObject* owner)
        : Component(owner)
    {
        m_Transform = &owner->GetTransform();
        m_Sprite = owner->GetComponent<SpriteSheetComponent>();
        assert(m_Transform && m_Sprite && "Player2BalloonComponent needs Transform + SpriteSheet");

        m_LastValidPosition = m_Transform->GetLocalPosition();

        // Set initial animation
        m_Sprite->ChangeAnimation(
            SpriteSheetComponent::AnimationState::MoveDown,
            1, 11, 0, 0, 6, 0.15f, true
        );
    }

    void Player2BalloonComponent::Update(float deltaTime)
    {
        // Handle invulnerability and flashing
        if (m_IsInvulnerable) {
            m_InvulTimer -= deltaTime;
            m_FlashTimer -= deltaTime;
            if (m_FlashTimer <= 0.f) {
                m_FlashTimer += m_FlashInterval;
                if (m_SpriteVisible) {
                    m_Sprite->Hide();
                    m_SpriteVisible = false;
                }
                else {
                    m_Sprite->Show();
                    m_SpriteVisible = true;
                }
            }
            if (m_InvulTimer <= 0.f) {
                m_IsInvulnerable = false;
                m_Sprite->Show();
                m_SpriteVisible = true;
            }
        }

        if (!m_IsDead) {
            MoveCurrent(deltaTime);
        }
        else {
            // Hide after death animation finishes
            if (m_Sprite->GetFrameCount() > 0 &&
                m_Sprite->GetCurrentFrame() == m_Sprite->GetFrameCount() - 1) {
                m_Sprite->Hide();
            }
        }

        // Clean up exploded bombs
        if (m_NeedsBombCleanup) {
            m_NeedsBombCleanup = false;
            std::vector<BombComponent*> validBombs;
            for (auto* bomb : m_ActiveBombs) {
                if (bomb && !bomb->IsExploded() && !bomb->IsMarkedForCleanup()) {
                    validBombs.push_back(bomb);
                }
            }
            auto removed = m_ActiveBombs.size() - validBombs.size();
            m_ActiveBombCount = std::max(0, m_ActiveBombCount - static_cast<int>(removed));
            m_ActiveBombs = std::move(validBombs);
        }
    }

    void Player2BalloonComponent::TakeDamage(int dmg)
    {
        (void)dmg;
        if (m_IsDead || m_IsInvulnerable) return;

        m_Lives--;
        std::cout << "Player 2 hit! Lives remaining: " << m_Lives << std::endl;
        ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_PLAYER_HIT, 1.0f);

        if (m_Lives <= 0) {
            m_IsDead = true;
            ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_PLAYER_DIE, 1.0f);
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::Death,
                1, 11, 0, 6, 5, 0.2f, false
            );

            // Notify VersusGameController that Player 2 died
            Notify({ GameEvents::PLAYER_DIED, 2 }); // 2 = Player 2
        }
        else {
            m_IsInvulnerable = true;
            m_InvulTimer = m_InvulDuration;
            m_FlashTimer = m_FlashInterval;
            m_SpriteVisible = true;
        }
    }

    void Player2BalloonComponent::OnMovementPressed(Direction dir)
    {
        if (m_IsDead) return;

        // Snap to grid like the player does
        constexpr float offsetX = 8.f, offsetY = 8.f;
        auto wp = GetOwner()->GetTransform().GetWorldPosition();
        wp.x -= offsetX;
        wp.y -= offsetY;

        if (dir == Direction::Up || dir == Direction::Down) {
            wp.x = std::floor(wp.x / m_TileSize + 0.5f) * m_TileSize;
        }
        else {
            wp.y = std::floor(wp.y / m_TileSize + 0.5f) * m_TileSize;
        }

        wp.x += offsetX;
        wp.y += offsetY;
        GetOwner()->GetTransform().SetLocalPosition(wp.x, wp.y, wp.z);
        m_LastValidPosition = GetOwner()->GetTransform().GetLocalPosition();

        if (std::find(m_MovementDirs.begin(), m_MovementDirs.end(), dir) == m_MovementDirs.end()) {
            m_MovementDirs.push_back(dir);
        }
        UpdateSpriteState();
    }

    void Player2BalloonComponent::OnMovementReleased(Direction dir)
    {
        if (m_IsDead) return;

        auto it = std::find(m_MovementDirs.begin(), m_MovementDirs.end(), dir);
        if (it != m_MovementDirs.end()) {
            m_MovementDirs.erase(it);
        }
        UpdateSpriteState();
    }

    void Player2BalloonComponent::BeginMove()
    {
        m_LastValidPosition = m_Transform->GetLocalPosition();
    }

    void Player2BalloonComponent::Move(float dx, float dy)
    {
        auto pos = m_Transform->GetLocalPosition();
        m_Transform->SetLocalPosition(pos.x + dx, pos.y + dy, pos.z);
    }

    void Player2BalloonComponent::RevertMove()
    {
        m_Transform->SetLocalPosition(m_LastValidPosition);
    }

    void Player2BalloonComponent::MoveCurrent(float dt)
    {
        if (m_MovementDirs.empty()) return;

        float amount = m_Speed * dt;
        BeginMove();

        switch (m_MovementDirs.back()) {
        case Direction::Left:  Move(-amount, 0.f); break;
        case Direction::Right: Move(amount, 0.f); break;
        case Direction::Up:    Move(0.f, -amount); break;
        case Direction::Down:  Move(0.f, amount); break;
        }
    }

    void Player2BalloonComponent::UpdateSpriteState()
    {
        if (m_MovementDirs.empty()) {
            // Idle state - just keep current animation going
            return;
        }

        // Update animation based on direction
        constexpr float frameDuration = 0.15f;
        switch (m_MovementDirs.back()) {
        case Direction::Left:
        case Direction::Right:
        case Direction::Up:
        case Direction::Down:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveDown,
                1, 11, 0, 0, 6, frameDuration, true
            );
            break;
        }
    }

    void Player2BalloonComponent::PlaceBomb(Scene& scene)
    {
        if (!CanPlaceBomb()) return;

        constexpr float bombOffsetX = 8.f;
        constexpr float bombOffsetY = 8.f;
        auto raw = GetOwner()->GetTransform().GetWorldPosition();
        raw.x -= bombOffsetX;
        raw.y -= bombOffsetY;

        glm::vec2 gridPos{
            std::floor(raw.x / m_TileSize + 0.5f) * m_TileSize,
            std::floor(raw.y / m_TileSize + 0.5f) * m_TileSize
        };

        SDL_Rect bombBox{ int(gridPos.x), int(gridPos.y), int(m_TileSize), int(m_TileSize) };

        // Check for collisions with walls
        for (auto* c : CollisionManager::GetInstance().GetComponents()) {
            auto* r = c->GetResponder();
            if ((dynamic_cast<StaticWallResponder*>(r) ||
                dynamic_cast<DestructibleWallResponder*>(r)) &&
                AABBIntersect(bombBox, c->GetBoundingBox())) {
                return; // blocked
            }
        }

        auto bombGO = std::make_shared<GameObject>();
        bombGO->AddComponent<TransformComponent>()
            .SetLocalPosition(gridPos.x + bombOffsetX, gridPos.y + bombOffsetY, 0.f);

        auto& bc = bombGO->AddComponent<BombComponent>();
        bc.Init("BombSpritesheet.tga", 3, 1, 0.2f, m_BombRange, 2.f, scene);

        // Add collision to the bomb
        auto& bombCollision = bombGO->AddComponent<CollisionComponent>();
        bombCollision.SetSize(m_TileSize * 0.8f, m_TileSize * 0.8f);
        bombCollision.SetOffset(m_TileSize * 0.1f, m_TileSize * 0.1f);
        bombCollision.SetResponder(std::make_unique<BombCollisionResponder>(&bc));

        // Subscribe to bomb events
        bc.AddObserver(this);

        // Track bomb
        m_ActiveBombs.push_back(&bc);
        m_ActiveBombCount++;

        scene.Add(bombGO);
        ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_BOMB_PLACE, 0.8f);
    }

    void Player2BalloonComponent::DetonateOldestBomb()
    {
        if (!m_HasDetonator || m_ActiveBombs.empty()) return;

        BombComponent* bombToDetonate = nullptr;
        for (auto* bomb : m_ActiveBombs) {
            if (bomb && !bomb->IsExploded()) {
                bombToDetonate = bomb;
                break;
            }
        }

        if (bombToDetonate) {
            bombToDetonate->ForceExplode();
            m_NeedsBombCleanup = true;
        }
    }

    void Player2BalloonComponent::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::BOMB_EXPLODED) {
            m_NeedsBombCleanup = true;
        }
    }
}