#include "GameOverManager.h"
#include "ScoreManager.h"  
#include "HighScoreManager.h"
#include "GameOverScreenLoader.h"
#include <iostream>
#include "PlayerNameManager.h"

namespace dae {
    void GameOverManager::TriggerGameOver()
    {
        int finalScore = ScoreManager::GetInstance().GetScore();
        std::string playerName = PlayerNameManager::GetInstance().GetPlayerName();

        std::cout << "Game Over! Player: " << playerName << " - Final Score: " << finalScore << std::endl;

        // Handle high scores using the player's entered name
        if (HighScoreManager::GetInstance().IsHighScore(finalScore)) {
            std::cout << "New High Score for " << playerName << "!" << std::endl;
            HighScoreManager::GetInstance().AddHighScore(playerName, finalScore);
        }

        // Display high scores to console (keep your existing logic)
        std::cout << "\n=== HIGH SCORES ===" << std::endl;
        const auto& highScores = HighScoreManager::GetInstance().GetHighScores();
        for (size_t i = 0; i < highScores.size(); ++i) {
            std::cout << i + 1 << ". " << highScores[i].name << " - " << highScores[i].score << std::endl;
        }

        // Load the game over screen
        GameOverScreenLoader loader;
        loader.LoadGameOverScreen();
    }
}