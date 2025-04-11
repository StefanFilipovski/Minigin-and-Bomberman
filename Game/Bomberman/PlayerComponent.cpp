// PlayerComponent.cpp
#include "PlayerComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"

namespace dae {

    PlayerComponent::PlayerComponent(GameObject* owner)
        : Component(owner), m_health(3), m_speed(100.f)
    {
        // Optionally initialize additional player properties
    }

    void PlayerComponent::Update(float deltaTime)
    {
        // Currently empty.
        // You can update additional state here as needed.
        (void)deltaTime;
    }

    void PlayerComponent::BeginMove()
    {
        // Save the current position as the last valid position before movement.
        m_lastValidPosition = GetOwner()->GetTransform().GetLocalPosition();
    }

    void PlayerComponent::Move(float dx, float dy)
    {
        // Fetch the current position via the TransformComponent.
        auto& transform = GetOwner()->GetTransform();
        glm::vec3 pos = transform.GetLocalPosition();
        pos.x += dx;
        pos.y += dy;
        transform.SetLocalPosition(pos.x, pos.y, pos.z);
    }

    void PlayerComponent::RevertMove()
    {
        // Restore the last valid position in the TransformComponent.
        GetOwner()->GetTransform().SetLocalPosition(m_lastValidPosition.x,
            m_lastValidPosition.y,
            m_lastValidPosition.z);
    }

    int PlayerComponent::GetHealth() const
    {
        return m_health;
    }

    void PlayerComponent::SetHealth(int health)
    {
        m_health = health;
    }

    void PlayerComponent::TakeDamage(int damage)
    {
        m_health -= damage;
        std::cout << "Player took " << damage
            << " damage. Health now: " << m_health << std::endl;

        if (m_health <= 0)
        {
            std::cout << "Player has died." << std::endl;
            // Here you might dispatch an event or handle death logic.
        }
    }

} // namespace dae
