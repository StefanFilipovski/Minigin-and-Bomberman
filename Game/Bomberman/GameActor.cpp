#include "GameActor.h"
#include "GameObject.h"
#include <iostream>
#include "GameEvents.h"

dae::GameActor::GameActor(GameObject* owner)
    : Component(owner)
    , m_Transform(owner->GetTransform())
    , m_Health(1) // Default health value
{
}

void dae::GameActor::Move(float x, float y)
{
    auto& transform = GetOwner()->GetTransform();
    glm::vec3 position = transform.GetLocalPosition();
    position.x += x;
    position.y += y;
    transform.SetLocalPosition(position);
}

void dae::GameActor::TakeDamage(int damage)
{
    
    if (m_Health <= 0)
        return;

    m_Health -= damage;
    std::cout << "GameActor took " << damage << " damage. Health now: " << m_Health << std::endl;
    if (m_Health <= 0)
    {
        Die();
    }
}


void dae::GameActor::Die()
{
    std::cout << "GameActor has died." << std::endl;
    Event event{ GameEvents::PLAYER_DIED };
    Notify(event);
}
