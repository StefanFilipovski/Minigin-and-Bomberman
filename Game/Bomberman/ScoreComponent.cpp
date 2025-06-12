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
        m_Font = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 16);
    }

    ScoreComponent::~ScoreComponent()
    {
        // Unregister from ScoreManager when destroyed
        if (ScoreManager::GetInstance().GetScoreDisplay() == this) {
            ScoreManager::GetInstance().SetScoreDisplay(nullptr);
        }
    }

    void ScoreComponent::Initialize()
    {
        // Defer initialization
        m_NeedsInit = true;
        m_InitTimer = 0.1f; // Wait 100ms before initializing
    }

    void ScoreComponent::Update(float deltaTime)
    {
        // Safety check
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

        // Handle deferred initialization
        if (m_NeedsInit) {
            m_InitTimer -= deltaTime;
            if (m_InitTimer <= 0.0f) {
                PerformInit();
            }
            return;
        }

        // Force refresh if needed
        if (m_NeedsRefresh && m_TextComponent) {
            int currentScore = ScoreManager::GetInstance().GetScore();
            if (m_LastDisplayedScore != currentScore) {
                SetScore(currentScore);
            }
            m_NeedsRefresh = false;
        }
    }

    void ScoreComponent::PerformInit()
    {
        // Safety check
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

        // Add text component if it doesn't exist
        m_TextComponent = GetOwner()->GetComponent<TextComponent>();
        if (!m_TextComponent) {
            auto& textComp = GetOwner()->AddComponent<TextComponent>(m_Font, "Score: 0");
            m_TextComponent = &textComp;
        }

        // Register with ScoreManager
        ScoreManager::GetInstance().SetScoreDisplay(this);

        // Display current score from ScoreManager
        SetScore(ScoreManager::GetInstance().GetScore());

        m_Initialized = true;
        m_NeedsInit = false;
    }

    void ScoreComponent::SetScore(int score)
    {
        // Safety checks
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

        if (!m_TextComponent) {
            m_NeedsRefresh = true;  // Try again later
            return;
        }

        // Only update if score actually changed
        if (m_LastDisplayedScore == score && m_Initialized) {
            return;
        }

        m_LastDisplayedScore = score;

        std::stringstream ss;
        ss << "Score: " << score;

        try {
            m_TextComponent->SetText(ss.str());
        }
        catch (...) {
            // If SetText fails, mark for refresh
            m_NeedsRefresh = true;
        }
    }
}