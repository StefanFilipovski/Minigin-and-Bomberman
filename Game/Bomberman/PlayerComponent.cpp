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
#include <SoundIds.h>
#include "BombCollisionResponder.h"
#include "GameOverManager.h"
#include "LivesManager.h"
#include "GameState.h"

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

    PlayerComponent::~PlayerComponent()
    {
        // Clean up bomb references on destruction
        m_ActiveBombs.clear();
    }

    void PlayerComponent::Update(float dt)
    {
        // Don't update during transitions
        if (GameStateManager::GetState() != GameState::Playing) {
            return;
        }

        // Safety check
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

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

            // Handle footstep timing
            if (!m_MovementDirs.empty()) {
                m_FootstepTimer += dt;
                if (m_FootstepTimer >= m_FootstepInterval) {
                    m_FootstepTimer = 0.0f;
                    ServiceLocator::GetSoundSystem().Play(
                        dae::SoundId::SOUND_FOOTSTEPS, 0.5f
                    );
                }
            }
            else {
                m_FootstepTimer = 0.0f;
            }
        }
        else {
            // Handle death animation
            if (!m_DeathAnimationComplete && m_Sprite) {
                try {
                    int frameCount = m_Sprite->GetFrameCount();
                    int currentFrame = m_Sprite->GetCurrentFrame();

                    if (frameCount > 0 && currentFrame >= 0 && currentFrame == frameCount - 1) {
                        m_DeathAnimationComplete = true;
                        m_Sprite->Hide();

                        // Make sure we're not already being deleted
                        if (!GetOwner()->IsMarkedForDeletion()) {
                            Notify({ GameEvents::PLAYER_DIED });
                        }
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Error in death animation: " << e.what() << std::endl;
                    m_DeathAnimationComplete = true;
                }
            }
        }

        // Clean up exploded bombs at a safe time
        if (m_NeedsBombCleanup) {
            m_NeedsBombCleanup = false;

            // Create a new vector with only valid bombs
            std::vector<BombComponent*> validBombs;

            for (auto* bomb : m_ActiveBombs) {
                if (bomb && bomb->GetOwner() && !bomb->GetOwner()->IsMarkedForDeletion() &&
                    !bomb->IsExploded() && !bomb->IsMarkedForCleanup()) {
                    validBombs.push_back(bomb);
                }
            }

            // Calculate how many we removed
            auto removed = m_ActiveBombs.size() - validBombs.size();
            m_ActiveBombCount = std::max(0, m_ActiveBombCount - static_cast<int>(removed));

            // Replace with the valid bombs
            m_ActiveBombs = std::move(validBombs);
        }
    }

    int PlayerComponent::GetHealth() const { return m_health; }

    void PlayerComponent::TakeDamage(int dmg)
    {
        (void)dmg; // Mark as unused

        if (m_IsDead || m_IsInvulnerable) return;

        std::cout << "Player hit!\n";
        ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_PLAYER_HIT, 1.0f);

        // Get current lives before the hit
        int currentLives = LivesManager::GetInstance().GetLives();

        // Notify about hit (this decreases lives)
        Notify({ GameEvents::PLAYER_HIT });

        // Check if this was the killing blow
        if (currentLives <= 1) { // Will be 0 after the hit
            m_IsDead = true;
            m_DeathAnimationComplete = false;  // Reset the flag

            // Clear bomb references immediately
            m_ActiveBombs.clear();
            m_ActiveBombCount = 0;

            ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_PLAYER_DIE, 1.0f);

            // Start death animation
            if (m_Sprite && !GetOwner()->IsMarkedForDeletion()) {
                m_Sprite->ChangeAnimation(
                    SpriteSheetComponent::AnimationState::Death,
                    3, 6, 2, 0, 6, 0.15f, false
                );
            }

            // Don't notify about death here - wait for animation to complete
        }
        else {
            // Just got hit, become invulnerable
            m_IsInvulnerable = true;
            m_InvulTimer = m_InvulDuration;
            m_FlashTimer = m_FlashInterval;
            m_SpriteVisible = true;
        }
    }

    void PlayerComponent::OnMovementPressed(Direction dir)
    {
        if (m_IsDead || GameStateManager::GetState() != GameState::Playing) return;

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
        if (!m_Sprite) return;

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
        // Check game state
        if (GameStateManager::GetState() != GameState::Playing) {
            return;
        }

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

        // Add collision to the bomb so it can be hit by blasts
        auto& bombCollision = bombGO->AddComponent<CollisionComponent>();
        bombCollision.SetSize(s_TileSize * 0.8f, s_TileSize * 0.8f);
        bombCollision.SetOffset(s_TileSize * 0.1f, s_TileSize * 0.1f);
        bombCollision.SetResponder(std::make_unique<BombCollisionResponder>(&bc));

        // Subscribe to bomb events
        bc.AddObserver(this);

        // Track bomb for detonator
        m_ActiveBombs.push_back(&bc);
        m_ActiveBombCount++;

        scene.Add(bombGO);

        ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_BOMB_PLACE, 0.8f);
    }

    void PlayerComponent::DetonateOldestBomb()
    {
        if (!m_HasDetonator || m_ActiveBombs.empty()) {
            return;
        }

        // Find the first non-exploded bomb
        BombComponent* bombToDetonate = nullptr;
        for (auto* bomb : m_ActiveBombs) {
            if (bomb && bomb->GetOwner() && !bomb->GetOwner()->IsMarkedForDeletion() &&
                !bomb->IsExploded()) {
                bombToDetonate = bomb;
                break;
            }
        }

        if (bombToDetonate) {
            // This will trigger chain reactions if other bombs are nearby
            bombToDetonate->ForceExplode();
            m_NeedsBombCleanup = true;
        }
    }

    void PlayerComponent::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::BOMB_EXPLODED) {
            // Just mark that we need to clean up bombs
            m_NeedsBombCleanup = true;
        }
    }

} // namespace dae