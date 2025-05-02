#include "PlayerComponent.h"
#include "GameObject.h"
#include "SpriteSheetComponent.h"
#include "TransformComponent.h"
#include "GameEvents.h"
#include <cassert>
#include <iostream>
#include <ServiceLocator.h>
#include "CollisionManager.h"

namespace dae {

    PlayerComponent::PlayerComponent(GameObject* owner)
        : Component(owner)
    {
        m_Sprite = owner->GetComponent<SpriteSheetComponent>();
        m_Transform = &owner->GetTransform();
        //Player component has to have these components to exist
        assert(m_Sprite && m_Transform);
    }

    void PlayerComponent::Update(float dt)
    {
        // invul + blink
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
            // hide when death anim finishes
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
        std::cout << "Player took " << dmg
            << " damage, health now " << m_health << "\n";
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
        if (std::find(m_MovementDirs.begin(), m_MovementDirs.end(), dir)
            == m_MovementDirs.end())
        {
            m_MovementDirs.push_back(dir);
            UpdateSpriteState();
        }
    }

    void PlayerComponent::OnMovementReleased(Direction dir)
    {
        if (m_IsDead) return;
        auto it = std::find(m_MovementDirs.begin(), m_MovementDirs.end(), dir);
        if (it != m_MovementDirs.end()) {
            m_MovementDirs.erase(it);
            UpdateSpriteState();
        }
    }

    void PlayerComponent::BeginMove()
    {
        m_lastValidPosition = m_Transform->GetLocalPosition();
    }

    void PlayerComponent::Move(float dx, float dy)
    {
        auto p = m_Transform->GetLocalPosition();
        p.x += dx; p.y += dy;
        m_Transform->SetLocalPosition(p.x, p.y, p.z);
    }

    void PlayerComponent::RevertMove()
    {
        auto& p = m_lastValidPosition;
        m_Transform->SetLocalPosition(p.x, p.y, p.z);
    }

    void PlayerComponent::MoveCurrent(float dt)
    {
        float a;
        a = dt;
        if (m_MovementDirs.empty()) return;
        float amt = m_speed * dt;
        BeginMove();

        switch (m_MovementDirs.back()) {
        case Direction::Left:  Move(-amt, 0); break;
        case Direction::Right: Move(+amt, 0); break;
        case Direction::Up:    Move(0, -amt); break;
        case Direction::Down:  Move(0, +amt); break;
        }

        dae::CollisionManager::GetInstance().CheckCollisions();
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

} 
