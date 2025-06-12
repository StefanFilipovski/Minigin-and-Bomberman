#include "GameOverManager.h"
#include "ScoreManager.h"  
#include "HighScoreManager.h"
#include "GameOverScreenLoader.h"
#include <iostream>

namespace dae {
    void GameOverManager::TriggerGameOver()
    {
        int finalScore = ScoreManager::GetInstance().GetScore();
        std::cout << "Game Over! Final Score: " << finalScore << std::endl;

        // Handle high scores (keep your existing logic)
        if (HighScoreManager::GetInstance().IsHighScore(finalScore)) {
            std::cout << "New High Score!" << std::endl;
            HighScoreManager::GetInstance().AddHighScore("AAA", finalScore);
        }

        // Display high scores to console (keep your existing logic)
        std::cout << "\n=== HIGH SCORES ===" << std::endl;
        const auto& highScores = HighScoreManager::GetInstance().GetHighScores();
        for (size_t i = 0; i < highScores.size(); ++i) {
            std::cout << i + 1 << ". " << highScores[i].name << " - " << highScores[i].score << std::endl;
        }

        // Load the game over screen instead of main menu
        GameOverScreenLoader loader;
        loader.LoadGameOverScreen();
    }
}