#include "ScoreComponent.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "ResourceManager.h"
#include <sstream>
#include "ScoreManager.h"

namespace dae {
    ScoreComponent::ScoreComponent(GameObject* owner)
        : Component(owner)
    {
        // Load font
        m_Font = ResourceManager::GetInstance().LoadFont("law-of-the-west.otf", 16);
    }

    void ScoreComponent::Initialize()
    {
        // Add text component if it doesn't exist
        m_TextComponent = GetOwner()->GetComponent<TextComponent>();
        if (!m_TextComponent) {
            GetOwner()->AddComponent<TextComponent>(m_Font, "Score: 0");
            m_TextComponent = GetOwner()->GetComponent<TextComponent>();
        }

        // Register with ScoreManager
        ScoreManager::GetInstance().SetScoreDisplay(this);

        // Display current score from ScoreManager
        SetScore(ScoreManager::GetInstance().GetScore());
    }

    void ScoreComponent::SetScore(int score)
    {
        if (m_TextComponent) {
            std::stringstream ss;
            ss << "Score: " << score;
            m_TextComponent->SetText(ss.str());
        }
    }
}