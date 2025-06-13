#include "GameModeSelector.h"
#include "NameEntryScreenLoader.h"
#include "VersusGameLoader.h"
#include "DelayedCoopGameLoader.h"
#include "LevelManager.h"
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
                // Note: Co-op mode is handled by DelayedCoopGameLoader, not here
            }
        }
    }

    void GameModeSelector::MoveUp()
    {
        switch (m_SelectedMode) {
        case GameMode::Story:
            m_SelectedMode = GameMode::Versus;
            std::cout << "Selected: Versus Mode" << std::endl;
            break;
        case GameMode::Coop:
            m_SelectedMode = GameMode::Story;
            std::cout << "Selected: Story Mode" << std::endl;
            break;
        case GameMode::Versus:
            m_SelectedMode = GameMode::Coop;
            std::cout << "Selected: Co-op Mode" << std::endl;
            break;
        }
    }

    void GameModeSelector::MoveDown()
    {
        switch (m_SelectedMode) {
        case GameMode::Story:
            m_SelectedMode = GameMode::Coop;
            std::cout << "Selected: Co-op Mode" << std::endl;
            break;
        case GameMode::Coop:
            m_SelectedMode = GameMode::Versus;
            std::cout << "Selected: Versus Mode" << std::endl;
            break;
        case GameMode::Versus:
            m_SelectedMode = GameMode::Story;
            std::cout << "Selected: Story Mode" << std::endl;
            break;
        }
    }

    void GameModeSelector::TriggerSelection()
    {
        std::cout << "Selection triggered - will execute in " << m_DelayTime << " seconds" << std::endl;
        m_ShouldLoadMode = true;
        m_DelayTimer = 0.0f;
    }

    void GameModeSelector::TriggerSelection(DelayedCoopGameLoader* coopLoader)
    {
        if (m_SelectedMode == GameMode::Coop && coopLoader) {
            // Use the delayed co-op loader for co-op mode
            std::cout << "Co-op mode selected - triggering delayed co-op loader" << std::endl;
            coopLoader->TriggerCoopGameLoad();
        }
        else {
            // Use the normal trigger for other modes
            TriggerSelection();
        }
    }
}