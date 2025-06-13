#include "DelayedCoopGameLoader.h"
#include "CoopLevelManager.h"
#include "ScoreManager.h"
#include <iostream>

namespace dae {
    void DelayedCoopGameLoader::Update(float deltaTime)
    {
        if (m_ShouldLoadCoopGame) {
            m_DelayTimer += deltaTime;
            if (m_DelayTimer >= m_DelayTime) {
                std::cout << "Delayed co-op game loading executing..." << std::endl;
                m_ShouldLoadCoopGame = false;

                // Initialize co-op level manager
                CoopLevelManager::GetInstance().Initialize();

                // Reset score
                ScoreManager::GetInstance().ResetScore();

                // Load first co-op level
                CoopLevelManager::GetInstance().LoadLevel(0);
            }
        }
    }

    void DelayedCoopGameLoader::TriggerCoopGameLoad()
    {
        std::cout << "Co-op game load triggered - will execute in " << m_DelayTime << " seconds" << std::endl;
        m_ShouldLoadCoopGame = true;
        m_DelayTimer = 0.0f;
    }
}