#include "ScoreManager.h"
#include "ScoreComponent.h"
#include "GameEvents.h"
#include "GameObject.h"
#include <iostream>

namespace dae {
    void ScoreManager::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::ENEMY_DIED) {
            std::cout << "ScoreManager received enemy died event with score: " << event.data << std::endl;
            AddScore(event.data);
        }
    }

    void ScoreManager::AddScore(int points)
    {
        m_TotalScore += points;
        std::cout << "Score added: " << points << ", Total: " << m_TotalScore << std::endl;
        UpdateDisplay();
    }

    void ScoreManager::UpdateDisplay()
    {
        if (m_CurrentDisplay && m_CurrentDisplay->GetOwner() &&
            !m_CurrentDisplay->GetOwner()->IsMarkedForDeletion()) {
            m_CurrentDisplay->SetScore(m_TotalScore);
        }
    }

    void ScoreManager::SetScoreDisplay(ScoreComponent* display)
    {
        m_CurrentDisplay = display;
        if (m_CurrentDisplay) {
            UpdateDisplay();
        }
    }

    void ScoreManager::ResetScore()
    {
        m_TotalScore = 0;
        UpdateDisplay();
    }
}