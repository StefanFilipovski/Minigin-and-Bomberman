#include "RestartChecker.h"
#include "LevelManager.h"
#include "ScoreManager.h"
#include <iostream>

namespace dae {
    bool RestartChecker::s_RestartRequested = false;

    void RestartChecker::Update(float deltaTime)
    {
        if (s_RestartRequested && !m_RestartTriggered) {
            m_DelayTimer += deltaTime;

            // Wait a tiny bit to ensure input processing is complete
            if (m_DelayTimer > 0.1f) {
                std::cout << "Executing restart..." << std::endl;
                m_RestartTriggered = true;
                s_RestartRequested = false;

                ScoreManager::GetInstance().ResetScore();
                LevelManager::GetInstance().ResetToFirstLevel();
            }
        }
    }
}