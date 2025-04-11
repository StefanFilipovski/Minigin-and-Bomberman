#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "Observer.h"  

namespace dae
{
    class GameActor : public Component, public Subject
    {
    public:
        
        GameActor(GameObject* owner);

        void Move(float x, float y);

        // Health management functions.
        void TakeDamage(int damage);
        void Die();

        // Setter and getter for health.
        void SetHealth(int health) { m_Health = health; }
        int GetHealth() const { return m_Health; }

    private:
        TransformComponent& m_Transform;
        int m_Health;
    };
}
