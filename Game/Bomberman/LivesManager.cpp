#include "LivesManager.h"
#include "LivesDisplayComponent.h"
#include "GameEvents.h"
#include "HighScoreManager.h"
#include "ScoreManager.h"
#include "LevelManager.h"
#include <iostream>
#include <SDL_timer.h>
#include "GameState.h"
#include "GameObject.h"

namespace dae {
    // Static variable to track if we have a pending timer
    static SDL_TimerID s_GameOverTimerID = 0;

    void LivesManager::OnNotify(const Event& event)
    {
        if (event.id == GameEvents::PLAYER_HIT) {
            SetLives(m_CurrentLives - 1);
        }
        else if (event.id == GameEvents::PLAYER_DIED) {
            // Cancel any existing timer
            if (s_GameOverTimerID != 0) {
                SDL_RemoveTimer(s_GameOverTimerID);
                s_GameOverTimerID = 0;
            }

            // Set transition state
            GameStateManager::SetState(GameState::GameOverTransition);

            // Create new timer with proper tracking
            s_GameOverTimerID = SDL_AddTimer(2000, // 2 second delay
                [](Uint32, void*) -> Uint32 {
                    // Clear the timer ID
                    s_GameOverTimerID = 0;

                    if (GameStateManager::GetState() == GameState::GameOverTransition) {
                        LivesManager::GetInstance().HandleGameOver();
                    }
                    return 0; // Don't repeat
                },
                nullptr
            );
        }
    }

    void LivesManager::SetLives(int lives)
    {
        m_CurrentLives = lives;
        UpdateDisplay();
    }

    void LivesManager::UpdateDisplay()
    {
        if (m_CurrentDisplay && m_CurrentDisplay->GetOwner() &&
            !m_CurrentDisplay->GetOwner()->IsMarkedForDeletion()) {
            m_CurrentDisplay->SetLives(std::max(0, m_CurrentLives));
        }
    }

    void LivesManager::HandleGameOver()
    {
        std::cout << "Game Over! No more lives." << std::endl;

        // Cancel any pending timer
        if (s_GameOverTimerID != 0) {
            SDL_RemoveTimer(s_GameOverTimerID);
            s_GameOverTimerID = 0;
        }

        // Get final score BEFORE any cleanup
        int finalScore = ScoreManager::GetInstance().GetScore();

        // Set state to prevent any updates during transition
        GameStateManager::SetState(GameState::GameOverTransition);

        // Clear display references FIRST to prevent invalid access
        ScoreManager::GetInstance().SetScoreDisplay(nullptr);
        SetLivesDisplay(nullptr);

        // Check and save high score
        if (HighScoreManager::GetInstance().IsHighScore(finalScore)) {
            std::cout << "New High Score: " << finalScore << "!" << std::endl;
            HighScoreManager::GetInstance().AddHighScore("AAA", finalScore);
        }

        // Display high scores
        std::cout << "\n=== HIGH SCORES ===" << std::endl;
        const auto& highScores = HighScoreManager::GetInstance().GetHighScores();
        for (size_t i = 0; i < highScores.size(); ++i) {
            std::cout << i + 1 << ". " << highScores[i].name << " - " << highScores[i].score << std::endl;
        }

        // Reset score and lives
        ScoreManager::GetInstance().ResetScore();
        ResetLives();

        // Restart from first level - this will also reset the game state
        LevelManager::GetInstance().ResetToFirstLevel();
    }

    LivesManager::~LivesManager()
    {
        // Clean up any pending timer on destruction
        if (s_GameOverTimerID != 0) {
            SDL_RemoveTimer(s_GameOverTimerID);
            s_GameOverTimerID = 0;
        }
    }
}