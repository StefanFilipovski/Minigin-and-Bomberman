#include "LivesDisplayComponent.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "ResourceManager.h"
#include "GameEvents.h"
#include <sstream>

namespace dae {
    LivesDisplayComponent::LivesDisplayComponent(GameObject* owner, int initialLives)
        : Component(owner)
        , m_CurrentLives(initialLives)
        , m_MaxLives(initialLives)
    {
        
        m_Font = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 16);
    }

    void LivesDisplayComponent::Initialize()
    {
        // Add text component if it doesn't exist
        m_TextComponent = GetOwner()->GetComponent<TextComponent>();
        if (!m_TextComponent) {
            GetOwner()->AddComponent<TextComponent>(m_Font, "Lives: " + std::to_string(m_CurrentLives));
            m_TextComponent = GetOwner()->GetComponent<TextComponent>();
        }

        UpdateDisplay();
    }

    // Update LivesDisplayComponent.cpp to handle the new life system:

    void LivesDisplayComponent::OnNotify(const Event& event)
    {
        switch (event.id) {
        case GameEvents::PLAYER_HIT:
            // Update lives display immediately when player gets hit
            // event.data contains the remaining lives
            SetLives(event.data);
            break;
        case GameEvents::PLAYER_DIED:
            // Player is completely out of lives
            SetLives(0);
            break;
        default:
            break;
        }
    }

    void LivesDisplayComponent::SetLives(int lives)
    {
        m_CurrentLives = lives;
        UpdateDisplay();
    }

    void LivesDisplayComponent::UpdateDisplay()
    {
        if (m_TextComponent) {
            std::stringstream ss;
            ss << "Lives: " << m_CurrentLives;
            m_TextComponent->SetText(ss.str());
        }
    }
}