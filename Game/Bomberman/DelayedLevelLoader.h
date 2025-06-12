#pragma once
#include "Component.h"
#include "LevelManager.h"
#include "ScoreManager.h"
#include <iostream>

namespace dae {
    class DelayedLevelLoader : public Component {
    public:
        DelayedLevelLoader(GameObject* owner) : Component(owner) {}

        void Update(float deltaTime) override {
            if (m_ShouldLoadLevel) {
                m_DelayTimer += deltaTime;
                if (m_DelayTimer >= m_DelayTime) {
                    std::cout << "Delayed level loading executing..." << std::endl;
                    m_ShouldLoadLevel = false;

                    // Reset score
                    ScoreManager::GetInstance().ResetScore();

                    // Load level
                    LevelManager::GetInstance().LoadLevel(0);
                }
            }
        }

        void TriggerLevelLoad() {
            std::cout << "Level load triggered - will execute in " << m_DelayTime << " seconds" << std::endl;
            m_ShouldLoadLevel = true;
            m_DelayTimer = 0.0f;
        }

    private:
        bool m_ShouldLoadLevel{ false };
        float m_DelayTimer{ 0.0f };
        float m_DelayTime{ 0.1f }; // 100ms delay
    };
}