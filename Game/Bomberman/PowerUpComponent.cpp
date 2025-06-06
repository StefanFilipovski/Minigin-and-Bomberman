#include "PowerUpComponent.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "GameEvents.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

namespace dae {
    PowerUpComponent::PowerUpComponent(GameObject* owner, PowerUpType type)
        : Component(owner), m_Type(type)
    {
    }

    void PowerUpComponent::Collect(PlayerComponent* player)
    {
        if (m_IsCollected) return;
        m_IsCollected = true;

        std::cout << "Collecting power-up type: " << static_cast<int>(m_Type) << std::endl;


        // Apply power-up to player
        switch (m_Type) {
        case PowerUpType::ExtraBomb:
            player->IncreaseBombCapacity();
            std::cout << "Increasing bomb capacity" << std::endl;
            break;
        case PowerUpType::Detonator:
            player->EnableDetonator();
            std::cout << "Enabling detonator" << std::endl;
            break;
        case PowerUpType::FlameRange:
            player->IncreaseBombRange();
            std::cout << "Increasing bomb range" << std::endl;

            break;
        }

        // Notify for UI/score
        Event powerUpEvent{ GameEvents::POWERUP_COLLECTED };
        Notify(powerUpEvent);

        // Hide the visual
        if (auto* renderer = GetOwner()->GetComponent<RenderComponent>()) {
            renderer->SetScale(0.0f, 0.0f);
        }

        // Move off-screen as backup
        if (auto* transform = &GetOwner()->GetTransform()) {
            transform->SetLocalPosition(-1000.f, -1000.f, 0.f);
        }

        // Mark for cleanup
       /* GetOwner()->MarkForDeletion();*/
    }
}