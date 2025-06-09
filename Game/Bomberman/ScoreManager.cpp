#include "ScoreManager.h"
#include "ScoreComponent.h"
#include "GameEvents.h"

namespace dae {
    void ScoreManager::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::ENEMY_DIED) {
            AddScore(event.data);
        }
    }

    void ScoreManager::AddScore(int points)
    {
        m_TotalScore += points;
        UpdateDisplay();
    }

    void ScoreManager::UpdateDisplay()
    {
        if (m_CurrentDisplay) {
            m_CurrentDisplay->SetScore(m_TotalScore);
        }
    }
}