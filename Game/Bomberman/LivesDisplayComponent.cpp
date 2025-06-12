#include "LivesDisplayComponent.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "ResourceManager.h"
#include <sstream>
#include "LivesManager.h"
#include "GameState.h"

namespace dae {
    LivesDisplayComponent::LivesDisplayComponent(GameObject* owner)
        : Component(owner)
    {
        // Load font
        m_Font = ResourceManager::GetInstance().LoadFont("PublicPixel-rv0pA.ttf", 16);
    }

    LivesDisplayComponent::~LivesDisplayComponent()
    {
        // Unregister from LivesManager when destroyed
        if (LivesManager::GetInstance().GetCurrentDisplay() == this) {
            LivesManager::GetInstance().SetLivesDisplay(nullptr);
        }
    }

    void LivesDisplayComponent::Initialize()
    {
        // Defer initialization by one frame to ensure renderer is ready
        m_NeedsInit = true;
    }

    void LivesDisplayComponent::Update(float deltaTime)
    {
        (void)deltaTime;

        // Don't update during transitions
        if (GameStateManager::IsTransitioning()) {
            return;
        }

        // Handle deferred initialization
        if (m_NeedsInit && !m_Initialized) {
            PerformInit();
        }

        // Force refresh if needed
        if (m_NeedsRefresh && m_TextComponent) {
            SetLives(LivesManager::GetInstance().GetLives());
            m_NeedsRefresh = false;
        }
    }

    void LivesDisplayComponent::PerformInit()
    {
        // Safety check
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

        // Add text component if it doesn't exist
        m_TextComponent = GetOwner()->GetComponent<TextComponent>();
        if (!m_TextComponent) {
            auto& textComp = GetOwner()->AddComponent<TextComponent>(m_Font, "Lives: 3");
            m_TextComponent = &textComp;
        }

        // Register with LivesManager
        LivesManager::GetInstance().SetLivesDisplay(this);

        // Display current lives from LivesManager
        SetLives(LivesManager::GetInstance().GetLives());

        m_Initialized = true;
        m_NeedsInit = false;
    }

    void LivesDisplayComponent::SetLives(int lives)
    {
        // Safety checks
        if (!GetOwner() || GetOwner()->IsMarkedForDeletion()) {
            return;
        }

        if (!m_TextComponent) {
            m_NeedsRefresh = true;  // Try again later
            return;
        }

        std::stringstream ss;
        ss << "Lives: " << lives;
        m_TextComponent->SetText(ss.str());
    }
}