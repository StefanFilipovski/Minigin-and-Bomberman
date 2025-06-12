#include "GameModeSelector.h"
#include "NameEntryScreenLoader.h"
#include "VersusGameLoader.h"
#include <iostream>

namespace dae {
    void GameModeSelector::Update(float deltaTime)
    {
        if (m_ShouldLoadMode) {
            m_DelayTimer += deltaTime;
            if (m_DelayTimer >= m_DelayTime) {
                std::cout << "Delayed mode loading executing..." << std::endl;
                m_ShouldLoadMode = false;

                if (m_SelectedMode == GameMode::Story) {
                    // Go to name entry for story mode
                    NameEntryScreenLoader loader;
                    loader.LoadNameEntryScreen();
                }
                else if (m_SelectedMode == GameMode::Versus) {
                    // Go directly to versus game
                    VersusGameLoader loader;
                    loader.LoadVersusGame();
                }
            }
        }
    }

    void GameModeSelector::MoveUp()
    {
        if (m_SelectedMode == GameMode::Versus) {
            m_SelectedMode = GameMode::Story;
            std::cout << "Selected: Story Mode" << std::endl;
        }
    }

    void GameModeSelector::MoveDown()
    {
        if (m_SelectedMode == GameMode::Story) {
            m_SelectedMode = GameMode::Versus;
            std::cout << "Selected: Versus Mode" << std::endl;
        }
    }

    void GameModeSelector::TriggerSelection()
    {
        std::cout << "Selection triggered - will execute in " << m_DelayTime << " seconds" << std::endl;
        m_ShouldLoadMode = true;
        m_DelayTimer = 0.0f;
    }
}