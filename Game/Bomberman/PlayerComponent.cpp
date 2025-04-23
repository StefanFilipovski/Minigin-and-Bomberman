#include "PlayerComponent.h"
#include "GameObject.h"
#include "SpriteSheetComponent.h"
#include "TransformComponent.h"
#include <algorithm>
#include <iostream>
#include <cassert>

namespace dae {

    PlayerComponent::PlayerComponent(GameObject* owner)
        : Component(owner)
    {
        // grab the already‐attached components
        m_Sprite = owner->GetComponent<SpriteSheetComponent>();
        // Safety check: we *must* have a SpriteSheetComponent
        assert(m_Sprite && "PlayerComponent requires a SpriteSheetComponent!");
              
        m_Transform = &owner->GetTransform();
        // TransformComponent is essential too
        assert(m_Transform && "PlayerComponent requires a TransformComponent!");
    }

    void PlayerComponent::Update(float deltaTime)
    {
        if (!m_IsDead)
        {
            // only move & collide if not dead
            MoveCurrent(deltaTime);
        }
        else
        {
            // if we’re dead, wait for the death animation to finish
            // then hide the sprite entirely.
            if (m_Sprite->GetFrameCount() > 0 &&
                m_Sprite->GetCurrentFrame() == m_Sprite->GetFrameCount() - 1)
            {
                m_Sprite->Hide();
            }
        }
    }

    void PlayerComponent::BeginMove()
    {
        m_lastValidPosition = m_Transform->GetLocalPosition();
    }

    void PlayerComponent::Move(float dx, float dy)
    {
        auto pos = m_Transform->GetLocalPosition();
        pos.x += dx; pos.y += dy;
        m_Transform->SetLocalPosition(pos.x, pos.y, pos.z);
    }

    void PlayerComponent::RevertMove()
    {
        m_Transform->SetLocalPosition(
            m_lastValidPosition.x,
            m_lastValidPosition.y,
            m_lastValidPosition.z
        );
    }

    int PlayerComponent::GetHealth() const
    {
        return m_health;
    }

    void PlayerComponent::TakeDamage(int dmg)
    {
        if (m_IsDead) return; // already dying/dead

        m_health -= dmg;
        std::cout << "Player took " << dmg
            << " damage. Health now: " << m_health << std::endl;

        if (m_health <= 0)
        {
            m_IsDead = true;
            std::cout << "Player has died; playing death animation.\n";

            // Kick off the 3×6 sheet, row 2, 6 frames, non-looping
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::Death,
                /*rows=*/3, /*cols=*/6,
                /*row=*/2,
                /*startCol=*/0, /*frameCount=*/6,
                /*dur=*/0.15f,
                /*loop=*/false
            );
        }
    }

    void PlayerComponent::OnInputPressed(SDL_Scancode key)
    {
        if (m_IsDead) return;

        // only allow arrows
        const SDL_Scancode keys[] = {
          SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
          SDL_SCANCODE_UP,   SDL_SCANCODE_DOWN
        };
        if (std::find(std::begin(keys), std::end(keys), key) == std::end(keys))
            return;

        if (std::find(m_MovementKeys.begin(), m_MovementKeys.end(), key)
            == m_MovementKeys.end())
        {
            m_MovementKeys.push_back(key);
        }
        UpdateSpriteState();
    }

    void PlayerComponent::OnInputReleased(SDL_Scancode key)
    {
        if (m_IsDead) return;

        auto it = std::find(m_MovementKeys.begin(), m_MovementKeys.end(), key);
        if (it != m_MovementKeys.end())
            m_MovementKeys.erase(it);

        UpdateSpriteState();
    }

    void PlayerComponent::MoveCurrent(float deltaTime)
    {
        if (m_MovementKeys.empty()) return;

        float amt = m_speed * deltaTime;
        BeginMove();

        switch (m_MovementKeys.back())
        {
        case SDL_SCANCODE_LEFT:  Move(-amt, 0.f); break;
        case SDL_SCANCODE_RIGHT: Move(+amt, 0.f); break;
        case SDL_SCANCODE_UP:    Move(0.f, -amt); break;
        case SDL_SCANCODE_DOWN:  Move(0.f, +amt); break;
        default: break;
        }

        //collision test here; if collision => RevertMove()
    }

    void PlayerComponent::UpdateSpriteState()
    {
        // if there’s no movement, go idle
        if (m_MovementKeys.empty())
        {
            m_Sprite->SetIdleFrame(
                SpriteSheetComponent::AnimationState::Idle,
                /*rows=*/3, /*cols=*/6,
                /*row=*/0, /*idleCol=*/4
            );
            return;
        }

        constexpr float FD = 0.15f;
        switch (m_MovementKeys.back())
        {
        case SDL_SCANCODE_LEFT:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveLeft,
                3, 6, 0, 0, 3, FD, true
            ); break;
        case SDL_SCANCODE_DOWN:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveDown,
                3, 6, 0, 3, 3, FD, true
            ); break;
        case SDL_SCANCODE_RIGHT:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveRight,
                3, 6, 1, 0, 3, FD, true
            ); break;
        case SDL_SCANCODE_UP:
            m_Sprite->ChangeAnimation(
                SpriteSheetComponent::AnimationState::MoveUp,
                3, 6, 1, 3, 3, FD, true
            ); break;
        default: break;
        }
    }

} // namespace dae
